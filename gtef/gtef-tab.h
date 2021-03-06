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

#ifndef GTEF_TAB_H
#define GTEF_TAB_H

#if !defined (GTEF_H_INSIDE) && !defined (GTEF_COMPILATION)
#error "Only <gtef/gtef.h> can be included directly."
#endif

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GTEF_TYPE_TAB (gtef_tab_get_type ())
G_DECLARE_DERIVABLE_TYPE (GtefTab, gtef_tab,
			  GTEF, TAB,
			  GtkGrid)

struct _GtefTabClass
{
	GtkGridClass parent_class;

	gpointer padding[12];
};

GtefTab *		gtef_tab_new					(GtkWidget *main_widget);

void			gtef_tab_add_info_bar				(GtefTab    *tab,
									 GtkInfoBar *info_bar);

G_END_DECLS

#endif /* GTEF_TAB_H */
