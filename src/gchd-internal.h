/*
 * Copyright (c) 2009     David Mohr <david@mcbf.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#ifndef _HAVE_GHCD_INTERNAL
#define _HAVE_GHCD_INTERNAL

#include <glib.h>

#include "gchd.h"

G_BEGIN_DECLS

typedef struct gchd_menu_struct {
  gchar * loc;
  gchar * name;
  GList * entries; /* of type GchdEntry * */
  gint n_entries;
  gboolean default_saved;
  struct gchd_menu_struct * parent;
} GchdMenu;

typedef struct {
    gchar * name;
    GchdMenu * submenu;
} GchdEntry;

struct _Gchd {
  /* virtual functions */
  gchar * (*get_default_entry) (Gchd * gchd, GError **error);
  gboolean (*set_default_entry) (Gchd * gchd, gchar * entry, GError **error);

  /* private data for the virtual functions */
  gpointer data;

  GchdMenu menu;
  gchar * default_entry;
  gchar * grub_dir;
  gboolean once;

  GchdSetDefaultCallback set_callback;
  gpointer set_callback_data;
};

gchar * gchd_get_grub_file (Gchd * gchd, const gchar * file, GError **error);

G_END_DECLS

#endif /* _HAVE_GHCD_INTERNAL */
