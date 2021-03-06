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
#ifndef _HAVE_GCHD
#define _HAVE_GCHD

#include <glib.h>

G_BEGIN_DECLS

typedef struct _Gchd Gchd;

/**
 * GchdSetDefaultCallback:
 * @gchd    : a #Gchd instance
 * @success : the success of setting the default
 * @error   : if success is %FALSE, then @error contains the reason
 * @data    : the data pointer that was added when the callback was set up
 *
 * Callback to pass to gchd_set_default_entry. If success is %FALSE, then
 * error contains a string describing the reason. Error should be freed by
 * the callback function.
 **/
typedef void (*GchdSetDefaultCallback) (Gchd *gchd, gboolean success, gchar * error, gpointer data);

Gchd * gchd_new (void);
void gchd_free (Gchd *gchd);

gint gchd_get_menu_entries (Gchd * gchd, GList **entries, GError **error);

gchar * gchd_get_default_entry (Gchd * gchd, GError **error);
gboolean gchd_set_default_entry (Gchd * gchd, gchar * entry, GchdSetDefaultCallback callback, gpointer data, GError **error);
gboolean gchd_uses_default (Gchd * gchd, GError ** error);
void gchd_set_once (Gchd * gchd, gboolean enable);

void gchd_set_grub_dir (Gchd * gchd, const gchar * grub_dir);
const gchar * gchd_get_grub_dir (Gchd * gchd);

G_END_DECLS

#endif /* _HAVE_GCHD */
