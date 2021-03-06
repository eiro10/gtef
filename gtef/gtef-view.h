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

#ifndef GTEF_VIEW_H
#define GTEF_VIEW_H

#if !defined (GTEF_H_INSIDE) && !defined (GTEF_COMPILATION)
#error "Only <gtef/gtef.h> can be included directly."
#endif

#include <gtksourceview/gtksource.h>

G_BEGIN_DECLS

#define GTEF_TYPE_VIEW (gtef_view_get_type ())
G_DECLARE_DERIVABLE_TYPE (GtefView, gtef_view,
			  GTEF, VIEW,
			  GtkSourceView)

struct _GtefViewClass
{
	GtkSourceViewClass parent_class;

	gpointer padding[12];
};

GtkWidget *		gtef_view_new					(void);

void			gtef_view_cut_clipboard				(GtefView *view);

void			gtef_view_copy_clipboard			(GtefView *view);

void			gtef_view_paste_clipboard			(GtefView *view);

void			gtef_view_delete_selection			(GtefView *view);

void			gtef_view_select_all				(GtefView *view);

void			gtef_view_scroll_to_cursor			(GtefView *view);

gboolean		gtef_view_goto_line				(GtefView *view,
									 gint      line);

gboolean		gtef_view_goto_line_offset			(GtefView *view,
									 gint      line,
									 gint      line_offset);

void			gtef_view_select_lines				(GtefView *view,
									 gint      start_line,
									 gint      end_line);

G_END_DECLS

#endif /* GTEF_VIEW_H */
