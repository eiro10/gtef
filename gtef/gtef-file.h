/*
 * This file is part of Gtef, a text editor library.
 *
 * Copyright 2016 - Sébastien Wilmet <swilmet@gnome.org>
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

#ifndef GTEF_FILE_H
#define GTEF_FILE_H

#if !defined (GTEF_H_INSIDE) && !defined (GTEF_COMPILATION)
#error "Only <gtef/gtef.h> can be included directly."
#endif

#include <gtksourceview/gtksource.h>
#include <gtef/gtef-types.h>

G_BEGIN_DECLS

#define GTEF_TYPE_FILE (gtef_file_get_type ())
G_DECLARE_DERIVABLE_TYPE (GtefFile, gtef_file,
			  GTEF, FILE,
			  GtkSourceFile)

struct _GtefFileClass
{
	GtkSourceFileClass parent_class;

	gpointer padding[12];
};

GtefFile *		gtef_file_new				(void);

GtefFileMetadata *	gtef_file_get_file_metadata		(GtefFile *file);

const gchar *		gtef_file_get_short_name		(GtefFile *file);

G_END_DECLS

#endif /* GTEF_FILE_H */
