/*
 * This file is part of Gtef, a text editor library.
 *
 * Copyright 2017 - Sébastien Wilmet <swilmet@gnome.org>
 *
 * Gtef is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at your
 * option) any later version.
 *
 * Gtef is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

#include "gtef-action-info-store.h"
#include "gtef-action-info.h"
#include "gtef-action-info-central-store.h"
#include "gtef-menu-item.h"

/**
 * SECTION:action-info-store
 * @Short_description: A store of GtefActionInfo's
 * @Title: GtefActionInfoStore
 * @See_also: #GtefActionInfo, #GtefActionInfoCentralStore
 *
 * #GtefActionInfoStore contains a set of #GtefActionInfo's.
 *
 * #GtefActionInfoStore is add-only, a #GtefActionInfo cannot be removed. If
 * needed, the remove operation will be added in the future.
 *
 * A #GtkApplication can be associated so that when a widget is created,
 * gtk_application_set_accels_for_action() is called. See
 * gtef_action_info_store_create_menu_item() for more details. Note that this
 * happens on widget creation, not when adding a #GtefActionInfo to the store,
 * so that the accelerator is bound to the application only if the
 * #GtefActionInfo is actually used.
 *
 * #GtefActionInfoStore is designed so that libraries can provide their own
 * store, to share action information (with translations) and possibly the
 * #GAction implementations as well. Application-specific #GtefActionInfo's can
 * be added to the store returned by
 * gtef_application_get_app_action_info_store().
 *
 * A library #GtefActionInfoStore must namespace the action names to not have
 * conflicts when a #GtefActionInfo is added to the #GtefActionInfoCentralStore.
 * Examples of namespaced action names: `"win.gtef-save"` or `"app.gtef-quit"`.
 */

struct _GtefActionInfoStorePrivate
{
	/* Weak ref, because usually GtkApplication owns (indirectly) a
	 * GtefActionInfoStore.
	 */
	GtkApplication *app;

	/* Key: owned gchar*: action name.
	 * Value: owned GtefActionInfo.
	 */
	GHashTable *hash_table;
};

enum
{
	PROP_0,
	PROP_APPLICATION,
	N_PROPERTIES
};

static GParamSpec *properties[N_PROPERTIES];

G_DEFINE_TYPE_WITH_PRIVATE (GtefActionInfoStore, gtef_action_info_store, G_TYPE_OBJECT)

static void
set_application (GtefActionInfoStore *store,
		 GtkApplication      *app)
{
	g_return_if_fail (app == NULL || GTK_IS_APPLICATION (app));

	g_assert (store->priv->app == NULL);

	if (app == NULL)
	{
		return;
	}

	store->priv->app = app;
	g_object_add_weak_pointer (G_OBJECT (store->priv->app),
				   (gpointer *) &store->priv->app);

	g_object_notify_by_pspec (G_OBJECT (store), properties[PROP_APPLICATION]);
}

static void
gtef_action_info_store_get_property (GObject    *object,
				     guint       prop_id,
				     GValue     *value,
				     GParamSpec *pspec)
{
	GtefActionInfoStore *store = GTEF_ACTION_INFO_STORE (object);

	switch (prop_id)
	{
		case PROP_APPLICATION:
			g_value_set_object (value, gtef_action_info_store_get_application (store));
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void
gtef_action_info_store_set_property (GObject      *object,
				     guint         prop_id,
				     const GValue *value,
				     GParamSpec   *pspec)
{
	GtefActionInfoStore *store = GTEF_ACTION_INFO_STORE (object);

	switch (prop_id)
	{
		case PROP_APPLICATION:
			set_application (store, g_value_get_object (value));
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void
gtef_action_info_store_dispose (GObject *object)
{
	GtefActionInfoStore *store = GTEF_ACTION_INFO_STORE (object);

	if (store->priv->app != NULL)
	{
		g_object_remove_weak_pointer (G_OBJECT (store->priv->app),
					      (gpointer *) &store->priv->app);
		store->priv->app = NULL;
	}

	G_OBJECT_CLASS (gtef_action_info_store_parent_class)->dispose (object);
}

static void
gtef_action_info_store_finalize (GObject *object)
{
	GtefActionInfoStore *store = GTEF_ACTION_INFO_STORE (object);

	g_hash_table_unref (store->priv->hash_table);

	G_OBJECT_CLASS (gtef_action_info_store_parent_class)->finalize (object);
}

static void
gtef_action_info_store_class_init (GtefActionInfoStoreClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->get_property = gtef_action_info_store_get_property;
	object_class->set_property = gtef_action_info_store_set_property;
	object_class->dispose = gtef_action_info_store_dispose;
	object_class->finalize = gtef_action_info_store_finalize;

	/**
	 * GtefActionInfoStore:application:
	 *
	 * The associated #GtkApplication. #GtefActionInfoStore has a weak
	 * reference to the #GtkApplication.
	 *
	 * Since: 2.0
	 */
	properties[PROP_APPLICATION] =
		g_param_spec_object ("application",
				     "GtkApplication",
				     "",
				     GTK_TYPE_APPLICATION,
				     G_PARAM_READWRITE |
				     G_PARAM_CONSTRUCT_ONLY |
				     G_PARAM_STATIC_STRINGS);

	g_object_class_install_properties (object_class, N_PROPERTIES, properties);
}

static void
gtef_action_info_store_init (GtefActionInfoStore *store)
{
	store->priv = gtef_action_info_store_get_instance_private (store);

	store->priv->hash_table = g_hash_table_new_full (g_str_hash,
							 g_str_equal,
							 g_free,
							 (GDestroyNotify) gtef_action_info_unref);
}

/**
 * gtef_action_info_store_new:
 * @application: (nullable): a #GtkApplication, or %NULL.
 *
 * Creates a new #GtefActionInfoStore object. Associating a #GtkApplication is
 * optional.
 *
 * Returns: a new #GtefActionInfoStore.
 * Since: 2.0
 */
GtefActionInfoStore *
gtef_action_info_store_new (GtkApplication *application)
{
	g_return_val_if_fail (application == NULL || GTK_IS_APPLICATION (application), NULL);

	return g_object_new (GTEF_TYPE_ACTION_INFO_STORE,
			     "application", application,
			     NULL);
}

/**
 * gtef_action_info_store_get_application:
 * @store: a #GtefActionInfoStore.
 *
 * Returns: (transfer none) (nullable): the associated #GtkApplication, or
 * %NULL.
 */
GtkApplication *
gtef_action_info_store_get_application (GtefActionInfoStore *store)
{
	g_return_val_if_fail (GTEF_IS_ACTION_INFO_STORE (store), NULL);

	return store->priv->app;
}

/**
 * gtef_action_info_store_add:
 * @store: a #GtefActionInfoStore.
 * @info: a #GtefActionInfo.
 *
 * Inserts @info into @store and into the #GtefActionInfoCentralStore. Both the
 * @store and central store must <emphasis>not</emphasis> already contain a
 * #GtefActionInfo with the same action name. The stores take their own
 * reference on @info.
 *
 * Since: 2.0
 */
void
gtef_action_info_store_add (GtefActionInfoStore *store,
			    GtefActionInfo      *info)
{
	const gchar *action_name;
	GtefActionInfoCentralStore *central_store;

	g_return_if_fail (GTEF_IS_ACTION_INFO_STORE (store));
	g_return_if_fail (info != NULL);

	action_name = gtef_action_info_get_action_name (info);
	g_return_if_fail (action_name != NULL);

	if (g_hash_table_lookup (store->priv->hash_table, action_name) != NULL)
	{
		g_warning ("%s(): the GtefActionInfoStore already contains a GtefActionInfo "
			   "with the action name “%s”.",
			   G_STRFUNC,
			   action_name);
		return;
	}

	g_hash_table_insert (store->priv->hash_table,
			     g_strdup (action_name),
			     gtef_action_info_ref (info));

	central_store = gtef_action_info_central_store_get_instance ();
	_gtef_action_info_central_store_add (central_store, info);
}

/**
 * gtef_action_info_store_add_entries:
 * @store: a #GtefActionInfoStore.
 * @entries: (array length=n_entries) (element-type GtefActionInfoEntry): a
 * pointer to the first item in an array of #GtefActionInfoEntry structs.
 * @n_entries: the length of @entries, or -1 if @entries is %NULL-terminated.
 * @translation_domain: (nullable): a gettext domain, or %NULL.
 *
 * Calls gtef_action_info_store_add() for each entry.
 *
 * If @translation_domain is not %NULL, g_dgettext() is used to translate the
 * @label and @tooltip of each entry before setting them to the #GtefActionInfo.
 *
 * An API similar to g_action_map_add_action_entries().
 *
 * Since: 2.0
 */
void
gtef_action_info_store_add_entries (GtefActionInfoStore       *store,
				    const GtefActionInfoEntry *entries,
				    gint                       n_entries,
				    const gchar               *translation_domain)
{
	gint i;

	g_return_if_fail (GTEF_IS_ACTION_INFO_STORE (store));
	g_return_if_fail (n_entries >= -1);
	g_return_if_fail (entries != NULL || n_entries == 0);

	for (i = 0; n_entries == -1 ? entries[i].action_name != NULL : i < n_entries; i++)
	{
		GtefActionInfo *info;

		info = gtef_action_info_new_from_entry (&entries[i], translation_domain);
		gtef_action_info_store_add (store, info);
		gtef_action_info_unref (info);
	}
}

/**
 * gtef_action_info_store_lookup:
 * @store: a #GtefActionInfoStore.
 * @action_name: an action name.
 *
 * Returns: (transfer none): the found #GtefActionInfo, or %NULL.
 * Since: 2.0
 */
const GtefActionInfo *
gtef_action_info_store_lookup (GtefActionInfoStore *store,
			       const gchar         *action_name)
{
	g_return_val_if_fail (GTEF_IS_ACTION_INFO_STORE (store), NULL);
	g_return_val_if_fail (action_name != NULL, NULL);

	return g_hash_table_lookup (store->priv->hash_table, action_name);
}

/**
 * gtef_action_info_store_create_menu_item:
 * @store: a #GtefActionInfoStore.
 * @action_name: an action name.
 *
 * Creates a new #GtkMenuItem for @action_name. The @store must contain a
 * #GtefActionInfo for @action_name.
 *
 * gtk_actionable_set_action_name() is called on the menu item with
 * @action_name. The label is set with the #GtkMenuItem:use-underline property
 * enabled. The first accelerator is set to the #GtkAccelLabel of the menu item.
 * The icon is set. And the tooltip is set with
 * gtef_menu_item_set_long_description().
 *
 * If #GtefActionInfoStore:application is non-%NULL, this function also calls
 * gtk_application_set_accels_for_action() with the accelerators returned by
 * gtef_action_info_get_accels() (this will erase previously set accelerators
 * for that action, if any).
 *
 * Returns: (transfer floating): a new #GtkMenuItem for @action_name.
 * Since: 2.0
 */
GtkWidget *
gtef_action_info_store_create_menu_item (GtefActionInfoStore *store,
					 const gchar         *action_name)
{
	GtkMenuItem *menu_item;
	GtefActionInfo *action_info;
	const gchar * const *accels;
	const gchar *icon_name;
	const gchar *tooltip;

	g_return_val_if_fail (GTEF_IS_ACTION_INFO_STORE (store), NULL);
	g_return_val_if_fail (action_name != NULL, NULL);

	action_info = g_hash_table_lookup (store->priv->hash_table, action_name);

	if (action_info == NULL)
	{
		g_warning ("%s(): action name '%s' not found.",
			   G_STRFUNC,
			   action_name);

		return NULL;
	}

	menu_item = GTK_MENU_ITEM (gtk_menu_item_new ());

	gtk_actionable_set_action_name (GTK_ACTIONABLE (menu_item), action_name);

	gtk_menu_item_set_use_underline (menu_item, TRUE);
	gtk_menu_item_set_label (menu_item, gtef_action_info_get_label (action_info));

	/* Set accel before setting icon, because
	 * gtef_menu_item_set_icon_name() adds a GtkBox.
	 */
	accels = gtef_action_info_get_accels (action_info);
	if (accels != NULL && accels[0] != NULL)
	{
		guint accel_key;
		GdkModifierType accel_mods;

		gtk_accelerator_parse (accels[0], &accel_key, &accel_mods);

		if (accel_key != 0 || accel_mods != 0)
		{
			GtkWidget *child;

			child = gtk_bin_get_child (GTK_BIN (menu_item));

			gtk_accel_label_set_accel (GTK_ACCEL_LABEL (child),
						   accel_key,
						   accel_mods);
		}
	}

	icon_name = gtef_action_info_get_icon_name (action_info);
	if (icon_name != NULL)
	{
		gtef_menu_item_set_icon_name (menu_item, icon_name);
	}

	tooltip = gtef_action_info_get_tooltip (action_info);
	if (tooltip != NULL)
	{
		gtef_menu_item_set_long_description (menu_item, tooltip);
	}

	if (store->priv->app != NULL)
	{
		gtk_application_set_accels_for_action (store->priv->app,
						       action_name,
						       accels);
	}

	_gtef_action_info_set_used (action_info);

	return GTK_WIDGET (menu_item);
}

static void
check_used_cb (gpointer key,
	       gpointer value,
	       gpointer user_data)
{
	const gchar *action_name = key;
	const GtefActionInfo *action_info = value;

	if (!_gtef_action_info_get_used (action_info))
	{
		g_warning ("GtefActionInfo with action_name='%s' has not been used.",
			   action_name);
	}
}

/**
 * gtef_action_info_store_check_all_used:
 * @store: a #GtefActionInfoStore.
 *
 * Checks that all #GtefActionInfo's of @store have been used by
 * gtef_action_info_store_create_menu_item(). If not, a warning is printed and
 * might indicate dead code.
 *
 * You probably want to call this function on the store returned by
 * gtef_application_get_app_action_info_store(). But it can also be useful for a
 * store provided by a library, to easily see which actions you don't use.
 *
 * Since: 2.0
 */
void
gtef_action_info_store_check_all_used (GtefActionInfoStore *store)
{
	g_return_if_fail (GTEF_IS_ACTION_INFO_STORE (store));

	g_hash_table_foreach (store->priv->hash_table,
			      check_used_cb,
			      NULL);
}
