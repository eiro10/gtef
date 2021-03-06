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

#include "gtef-action-info-central-store.h"
#include "gtef-action-info.h"

/**
 * SECTION:action-info-central-store
 * @Short_description: Aggregation of all GtefActionInfoStore's
 * @Title: GtefActionInfoCentralStore
 * @See_also: #GtefActionInfoStore
 *
 * #GtefActionInfoCentralStore is a singleton class containing the aggregation
 * of all #GtefActionInfoStore's. Each time a #GtefActionInfo is added to a
 * #GtefActionInfoStore, it is also added to the #GtefActionInfoCentralStore.
 */

/* API design:
 *
 * Why both GtefActionInfoStore and GtefActionInfoCentralStore are needed?
 *
 * Advantages of GtefActionInfoStore:
 * - gtef_action_info_store_new() takes an optional GtkApplication parameter. It
 *   doesn't rely on g_application_get_default() (calling
 *   g_application_get_default() in a library is not really a good practice I
 *   think. In theory an app can have several GApplication instances).
 * - gtef_action_info_store_check_all_used()
 *
 * Advantages of GtefActionInfoCentralStore:
 * - The central store checks if there are no duplicated action names
 *   (globally).
 * - [For the menu bar, easy to retrieve the tooltip to show it in the
 *   statusbar.] No longer relevant with gtef_menu_item_get_long_description().
 *
 * If there was only one of the two classes, hacks would be needed to achieve
 * the above items. So by having the two classes, we have the best of both
 * worlds. We should not be afraid to create a lot of classes, and see things in
 * big.
 */

struct _GtefActionInfoCentralStorePrivate
{
	/* Key: owned gchar*: action name.
	 * Value: owned GtefActionInfo.
	 */
	GHashTable *hash_table;
};

G_DEFINE_TYPE_WITH_PRIVATE (GtefActionInfoCentralStore, gtef_action_info_central_store, G_TYPE_OBJECT)

static void
gtef_action_info_central_store_finalize (GObject *object)
{
	GtefActionInfoCentralStore *central_store = GTEF_ACTION_INFO_CENTRAL_STORE (object);

	g_hash_table_unref (central_store->priv->hash_table);

	G_OBJECT_CLASS (gtef_action_info_central_store_parent_class)->finalize (object);
}

static void
gtef_action_info_central_store_class_init (GtefActionInfoCentralStoreClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->finalize = gtef_action_info_central_store_finalize;
}

static void
gtef_action_info_central_store_init (GtefActionInfoCentralStore *central_store)
{
	central_store->priv = gtef_action_info_central_store_get_instance_private (central_store);

	central_store->priv->hash_table = g_hash_table_new_full (g_str_hash,
								 g_str_equal,
								 g_free,
								 (GDestroyNotify) gtef_action_info_unref);
}

/**
 * gtef_action_info_central_store_get_instance:
 *
 * Returns: (transfer none): the #GtefActionInfoCentralStore singleton instance.
 * Since: 2.0
 */
GtefActionInfoCentralStore *
gtef_action_info_central_store_get_instance (void)
{
	static GtefActionInfoCentralStore *instance = NULL;

	if (G_UNLIKELY (instance == NULL))
	{
		instance = g_object_new (GTEF_TYPE_ACTION_INFO_CENTRAL_STORE, NULL);
	}

	return instance;
}

void
_gtef_action_info_central_store_add (GtefActionInfoCentralStore *central_store,
				     GtefActionInfo             *info)
{
	const gchar *action_name;

	g_return_if_fail (GTEF_IS_ACTION_INFO_CENTRAL_STORE (central_store));
	g_return_if_fail (info != NULL);

	action_name = gtef_action_info_get_action_name (info);
	g_return_if_fail (action_name != NULL);

	if (g_hash_table_lookup (central_store->priv->hash_table, action_name) != NULL)
	{
		g_warning ("The GtefActionInfoCentralStore already contains a GtefActionInfo "
			   "with the action name “%s”. Libraries must namespace their action names.",
			   action_name);
		return;
	}

	g_hash_table_insert (central_store->priv->hash_table,
			     g_strdup (action_name),
			     gtef_action_info_ref (info));
}

/**
 * gtef_action_info_central_store_lookup:
 * @central_store: a #GtefActionInfoCentralStore.
 * @action_name: an action name.
 *
 * Returns: (transfer none): the found #GtefActionInfo, or %NULL.
 * Since: 2.0
 */
const GtefActionInfo *
gtef_action_info_central_store_lookup (GtefActionInfoCentralStore *central_store,
				       const gchar                *action_name)
{
	g_return_val_if_fail (GTEF_IS_ACTION_INFO_CENTRAL_STORE (central_store), NULL);
	g_return_val_if_fail (action_name != NULL, NULL);

	return g_hash_table_lookup (central_store->priv->hash_table, action_name);
}
