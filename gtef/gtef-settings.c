/*
 * This file is part of Gtef, a text editor library.
 *
 * Copyright 2016 - Sébastien Wilmet <swilmet@gnome.org>
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

#include "gtef-settings.h"
#include <gtksourceview/gtksource.h>

static gboolean
strv_is_empty (gchar **strv)
{
	if (strv == NULL || strv[0] == NULL)
	{
		return TRUE;
	}

	/* Contains one empty string. */
	if (strv[1] == NULL && strv[0][0] == '\0')
	{
		return TRUE;
	}

	return FALSE;
}

static GSList *
encoding_strv_to_list (const gchar * const *encoding_strv)
{
	GSList *list = NULL;
	gchar **p;

	for (p = (gchar **)encoding_strv; p != NULL && *p != NULL; p++)
	{
		const gchar *charset = *p;
		const GtkSourceEncoding *encoding;

		/* TODO check if charset is equal to CURRENT. */

		encoding = gtk_source_encoding_get_from_charset (charset);

		if (encoding != NULL &&
		    g_slist_find (list, encoding) == NULL)
		{
			list = g_slist_prepend (list, (gpointer)encoding);
		}
	}

	return g_slist_reverse (list);
}

/* Take in priority the candidate encodings from GSettings. If the gsetting is
 * empty, take the default candidates of GtkSourceEncoding.
 * Also, ensure that UTF-8 and the current locale encoding are present.
 * Returns: a list of GtkSourceEncodings. Free with g_slist_free().
 */
GSList *
_gtef_settings_get_candidate_encodings (gboolean *default_candidates)
{
	const GtkSourceEncoding *utf8_encoding;
	const GtkSourceEncoding *current_encoding;
	GSettings *settings;
	gchar **settings_strv;
	GSList *candidates;

	utf8_encoding = gtk_source_encoding_get_utf8 ();
	current_encoding = gtk_source_encoding_get_current ();

	settings = g_settings_new ("org.gnome.gtef.preferences");

	settings_strv = g_settings_get_strv (settings, GTEF_SETTINGS_CANDIDATE_ENCODINGS);

	if (strv_is_empty (settings_strv))
	{
		if (default_candidates != NULL)
		{
			*default_candidates = TRUE;
		}

		candidates = gtk_source_encoding_get_default_candidates ();
	}
	else
	{
		if (default_candidates != NULL)
		{
			*default_candidates = FALSE;
		}

		candidates = encoding_strv_to_list ((const gchar * const *) settings_strv);

		/* Ensure that UTF-8 is present. */
		if (utf8_encoding != current_encoding &&
		    g_slist_find (candidates, utf8_encoding) == NULL)
		{
			candidates = g_slist_prepend (candidates, (gpointer)utf8_encoding);
		}

		/* Ensure that the current locale encoding is present (if not
		 * present, it must be the first encoding).
		 */
		if (g_slist_find (candidates, current_encoding) == NULL)
		{
			candidates = g_slist_prepend (candidates, (gpointer)current_encoding);
		}
	}

	g_object_unref (settings);
	g_strfreev (settings_strv);
	return candidates;
}
