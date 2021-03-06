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

#include "gtef-application.h"
#include "gtef-action-info-store.h"

/**
 * SECTION:application
 * @Short_description: An extension of GtkApplication
 * @Title: GtefApplication
 * @See_also: #GtefActionInfoStore
 *
 * #GtefApplication extends the #GtkApplication class.
 *
 * Note that #GtefApplication extends the #GtkApplication class but without
 * subclassing it, because several libraries might want to extend
 * #GtkApplication and an application needs to be able to use all those
 * extensions at the same time.
 */

struct _GtefApplicationPrivate
{
	GtkApplication *gtk_app;
	GtefActionInfoStore *app_action_info_store;
};

enum
{
	PROP_0,
	PROP_APPLICATION,
	N_PROPERTIES
};

#define GTEF_APPLICATION_KEY "gtef-application-key"

static GParamSpec *properties[N_PROPERTIES];

G_DEFINE_TYPE_WITH_PRIVATE (GtefApplication, gtef_application, G_TYPE_OBJECT)

static void
init_app_action_info_store (GtefApplication *gtef_app)
{
	g_return_if_fail (gtef_app->priv->app_action_info_store == NULL);
	g_assert (gtef_app->priv->gtk_app != NULL);

	gtef_app->priv->app_action_info_store = gtef_action_info_store_new (gtef_app->priv->gtk_app);
}

static void
gtef_application_get_property (GObject    *object,
			       guint       prop_id,
			       GValue     *value,
			       GParamSpec *pspec)
{
	GtefApplication *gtef_app = GTEF_APPLICATION (object);

	switch (prop_id)
	{
		case PROP_APPLICATION:
			g_value_set_object (value, gtef_application_get_application (gtef_app));
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void
gtef_application_set_property (GObject      *object,
			       guint         prop_id,
			       const GValue *value,
			       GParamSpec   *pspec)
{
	GtefApplication *gtef_app = GTEF_APPLICATION (object);

	switch (prop_id)
	{
		case PROP_APPLICATION:
			g_assert (gtef_app->priv->gtk_app == NULL);
			gtef_app->priv->gtk_app = g_value_get_object (value);

			init_app_action_info_store (gtef_app);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void
gtef_application_dispose (GObject *object)
{
	GtefApplication *gtef_app = GTEF_APPLICATION (object);

	gtef_app->priv->gtk_app = NULL;
	g_clear_object (&gtef_app->priv->app_action_info_store);

	G_OBJECT_CLASS (gtef_application_parent_class)->dispose (object);
}

static void
gtef_application_class_init (GtefApplicationClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->get_property = gtef_application_get_property;
	object_class->set_property = gtef_application_set_property;
	object_class->dispose = gtef_application_dispose;

	/**
	 * GtefApplication:application:
	 *
	 * The #GtkApplication.
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
gtef_application_init (GtefApplication *gtef_app)
{
	gtef_app->priv = gtef_application_get_instance_private (gtef_app);
}

/**
 * gtef_application_get_from_gtk_application:
 * @gtk_app: a #GtkApplication.
 *
 * Returns the #GtefApplication of @gtk_app. The returned object is guaranteed
 * to be the same for the lifetime of @gtk_app.
 *
 * Returns: (transfer none): the #GtefApplication of @gtk_app.
 * Since: 2.0
 */
GtefApplication *
gtef_application_get_from_gtk_application (GtkApplication *gtk_app)
{
	GtefApplication *gtef_app;

	g_return_val_if_fail (GTK_IS_APPLICATION (gtk_app), NULL);

	gtef_app = g_object_get_data (G_OBJECT (gtk_app), GTEF_APPLICATION_KEY);

	if (gtef_app == NULL)
	{
		gtef_app = g_object_new (GTEF_TYPE_APPLICATION,
					 "application", gtk_app,
					 NULL);

		g_object_set_data_full (G_OBJECT (gtk_app),
					GTEF_APPLICATION_KEY,
					gtef_app,
					g_object_unref);
	}

	g_return_val_if_fail (GTEF_IS_APPLICATION (gtef_app), NULL);
	return gtef_app;
}

/**
 * gtef_application_get_default:
 *
 * Convenience function that calls g_application_get_default() followed by
 * gtef_application_get_from_gtk_application(). The object returned by
 * g_application_get_default() must be a #GtkApplication.
 *
 * Returns: (transfer none): the default #GtefApplication.
 * Since: 2.0
 */
GtefApplication *
gtef_application_get_default (void)
{
	GApplication *g_app;

	g_app = g_application_get_default ();
	g_return_val_if_fail (GTK_IS_APPLICATION (g_app), NULL);

	return gtef_application_get_from_gtk_application (GTK_APPLICATION (g_app));
}

/**
 * gtef_application_get_application:
 * @gtef_app: a #GtefApplication.
 *
 * Returns: (transfer none): the #GtkApplication of @gtef_app.
 * Since: 2.0
 */
GtkApplication *
gtef_application_get_application (GtefApplication *gtef_app)
{
	g_return_val_if_fail (GTEF_IS_APPLICATION (gtef_app), NULL);

	return gtef_app->priv->gtk_app;
}

/**
 * gtef_application_get_app_action_info_store:
 * @gtef_app: a #GtefApplication.
 *
 * Returns an initially empty #GtefActionInfoStore reserved for the
 * application-specific actions. Libraries should not add #GtefActionInfo's to
 * this store. Libraries should provide their own store if they want to share
 * #GtefActionInfo's.
 *
 * Returns: (transfer none): the #GtefActionInfoStore reserved for the
 * application.
 * Since: 2.0
 */
GtefActionInfoStore *
gtef_application_get_app_action_info_store (GtefApplication *gtef_app)
{
	g_return_val_if_fail (GTEF_IS_APPLICATION (gtef_app), NULL);

	return gtef_app->priv->app_action_info_store;
}

/**
 * gtef_application_open_simple:
 * @gtef_app: a #GtefApplication.
 * @file: a #GFile.
 *
 * Calls g_application_open() with a single file and an empty hint.
 *
 * Since: 2.0
 */
void
gtef_application_open_simple (GtefApplication *gtef_app,
			      GFile           *file)
{
	GFile *files[1];

	g_return_if_fail (GTEF_IS_APPLICATION (gtef_app));
	g_return_if_fail (G_IS_FILE (file));

	files[0] = file;
	g_application_open (G_APPLICATION (gtef_app->priv->gtk_app), files, 1, "");
}

/* ex:set ts=8 noet: */
