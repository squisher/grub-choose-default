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

#include <config.h>

#include <glib.h>

#include "gchd.h"
#include "gchd-error.h"
#include "gchd-menu.h"
#include "gchd-util.h"
#if DIRECT==1
#  include "gchd-direct.h"
#else
#  include "gchd-unix.h"
#endif

static const gchar * grub_config_locations[] = {
  "/boot/grub",
  "/grub",
  NULL
};


Gchd *
gchd_new (void)
{
  Gchd *gchd;
  
  gchd = g_new0 (Gchd, 1);

#if DIRECT==1
  gchd_direct_init (gchd);
#else
  gchd_unix_init (gchd);
#endif

  return gchd;
}

void
gchd_free (Gchd *gchd)
{
  gchd_menu_free (&(gchd->menu));

  g_free (gchd);
}

gint
gchd_get_menu_entries (Gchd *gchd, GList **entries, GError **error)
{
  gboolean r;

  g_assert (entries != NULL);

  r = gchd_get_menu (gchd, error);

  if (r)
  {
    /* maybe we should copy this? */
    *entries = gchd->menu.entries;

    return gchd->menu.n_entries;
  }
  else
  {
    return -1;
  }
}

gchar *
gchd_get_grub_file (Gchd * gchd, const gchar * file, GError **error)
{
  gchar * cfg;
  const gchar ** base;
  gboolean r;

  g_assert (error == NULL || *error == NULL);
  g_assert (file != NULL);

  for (base = grub_config_locations; *base != NULL; base++)
  {
    cfg = g_build_filename (*base, file, NULL);

    DBG ("Looking for %s...", cfg);

    r = g_file_test (cfg, G_FILE_TEST_EXISTS);

    if (r)
    {
      break;
    }

    g_free (cfg);
  }

  if (r)
  {
    /* we found a file */
    gchd->grub_dir = *base;
    return cfg;
  }
  else
  {
    /* no file found */
    g_set_error (error, GCHD_ERROR,
                 GCHD_ERROR_FILE_NOT_FOUND,
                 "Could not find %s", file);
    return NULL;
  }
}


/* interface functions */

gchar *
gchd_get_default_entry (Gchd * gchd, GError **error)
{
  g_assert (gchd->get_default_entry != NULL);
  g_assert (!error || !*error);

  return gchd->get_default_entry (gchd, error);
}

gboolean
gchd_set_default_entry (Gchd * gchd, gchar * entry, GError **error)
{
  g_assert (gchd->set_default_entry != NULL);
  g_assert (!error || !*error);

  return gchd->set_default_entry (gchd, entry, error);
}
