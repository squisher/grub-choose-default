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

#include "gchd-direct.h"

static const gchar * default_key = "saved_entry";


/* prototypes */

static gchar * get_default_entry (Gchd * gchd, GError **error);
static gboolean set_default_entry (Gchd * gchd, gchar * entry, GError **error);


/* implementations */

void
gcd_direct_init (Gchd * gchd)
{
  g_debug ("Using direct method to set and get default entries");

  gchd->get_default_entry = get_default_entry;
  gchd->set_default_entry = set_default_entry;
}

static gchar *
get_default_entry (Gchd * gchd, GError **error)
{
  g_critical ("not implemented yet");
}

static gboolean
set_default_entry (Gchd * gchd, gchar * entry, GError **error)
{
  g_critical ("not implemented yet");
}
