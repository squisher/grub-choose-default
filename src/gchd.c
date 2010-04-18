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

#if HAVE_STRING_H
#  include <string.h>
#endif

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
#ifdef G_OS_WIN32
#  include <gio/gio.h>
#endif

static const gchar * grub_config_locations[] = {
  "boot" G_DIR_SEPARATOR_S "grub",
  "grub",
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

  g_free (gchd->grub_dir);
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
gchd_get_grub_file_from_root (Gchd * gchd, const gchar * root, const gchar * file, GError **error)
{
  gchar * cfg;
  const gchar ** base;
  gboolean r;

  for (base = grub_config_locations; *base != NULL; base++)
  {
    cfg = g_build_filename (root, *base, file, NULL);

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
    gchd->grub_dir = g_build_filename (root, *base, NULL);
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

gchar *
gchd_get_grub_file (Gchd * gchd, const gchar * file, GError **error)
{
#ifdef G_OS_WIN32
  GVolumeMonitor * volmon;
  GList * mounts, * iter;
#endif

  gchar * cfg;

  g_assert (error == NULL || *error == NULL);
  g_assert (file != NULL);

  if (gchd->grub_dir)
  {
    /* if we have the directory cached, then no need to find it again */

    return g_build_filename (gchd->grub_dir, file, NULL);
  }

#ifdef G_OS_WIN32
  volmon = g_volume_monitor_get ();
  mounts = g_volume_monitor_get_mounts (volmon);

  /* seems like gio returns the list in reverse order */
  mounts = g_list_reverse (mounts);

  cfg = NULL;
  g_set_error (error, GCHD_ERROR,
               GCHD_ERROR_NO_VOLUMES,
               "No volumes were found in the system");

  for (iter = mounts; iter != NULL && cfg == NULL; iter = g_list_next (iter))
  {
    GMount * mnt = iter->data;
    GFile * f_root;
    gchar * root;

    f_root = g_mount_get_root (mnt);
    root = g_file_get_path (f_root);

    if (strcmp (root, "A:\\") != 0)
    {
      DBG ("Using root %s", root);

      if (error && *error)
      {
        g_error_free (*error);
        *error = NULL;
      }

      cfg = gchd_get_grub_file_from_root (gchd, root, file, error);

      g_free (root);
      g_object_unref (f_root);
    }
    else
    {
      DBG ("Not checking A:\\");
    }
  }

  g_list_foreach (mounts, (GFunc) g_object_unref, NULL);
  g_list_free (mounts);
  g_object_unref (volmon);
#else
  cfg = gchd_get_grub_file_from_root (gchd, "/", file, error);
#endif

  return cfg;
}

void
gchd_set_grub_dir (Gchd * gchd, gchar * grub_dir)
{
  g_free (gchd->grub_dir);
  gchd->grub_dir = g_strdup (grub_dir);
}

const gchar *
gchd_get_grub_dir (Gchd * gchd)
{
  return gchd->grub_dir;
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
