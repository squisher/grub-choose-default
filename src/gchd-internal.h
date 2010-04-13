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

typedef struct {
  gchar * loc;
  GList * entries; /* of type char* */
  gint n_entries;
} GchdMenu;

struct _Gchd {
  gchar * (*get_default_entry) (Gchd * gchd, GError **error);
  gboolean (*set_default_entry) (Gchd * gchd, gchar * entry, GError **error);

  gpointer * data;

  GchdMenu menu;
  gchar * default_entry;
};

G_END_DECLS

#endif /* _HAVE_GHCD_INTERNAL */
