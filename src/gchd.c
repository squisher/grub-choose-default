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

#include "gchd-messages.h"

static const gchar * grub_config_locations[] = {
  "boot" G_DIR_SEPARATOR_S "grub",
  "grub",
  NULL
};

/**
 * gchd_new:
 *
 * Returns a new #Gchd object.
 **/
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

/**
 * gchd_free:
 * @gchd  : a #Gchd.
 *
 * Frees the @gchd and its contents.
 **/
void
gchd_free (Gchd *gchd)
{
  gchd_menu_free (&(gchd->menu));

  g_free (gchd->grub_dir);
  g_free (gchd->data);
  g_free (gchd);
}

/**
 * gchd_get_menu_entries:
 * @gchd    : a #Gchd.
 * @entries : the location where the list of entries will be stored.
 * @error   : the error to be set if the entries could not be located.
 *
 * Determines the entries contained in grub.cfg, and stores them in @entries.
 * These entries are owned by @gchd and should not be modified.
 *
 * Returns: the number of entries, or
 *          -1 on error.
 **/
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

/**
 * gchd_get_grub_file_from_root:
 * @gchd    : a #Gchd.
 * @root    : the root of the filesystem to search in as a string.
 * @file    : the file name to search for as a string.
 * @error   : the error to set if unexpected issues occurred.
 *
 * Searches in predetermined subdirectories for @file.
 *
 * Returns: the full path to the @file as a string to be freed by the caller, or
 *          %NULL if no file was found, and error is set accordingly.
 **/
gchar *
gchd_get_grub_file_from_root (Gchd * gchd, const gchar * root, const gchar * file, GError **error)
{
  gchar * cfg;
  const gchar ** base;
  gboolean r;

  g_assert (error == NULL || *error == NULL);

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

/**
 * gchd_get_grub_file:
 * @gchd  : a #Gchd.
 * @file  : the file to open in the grub directory.
 * @error : the error to be set if the file could not be opened.
 *
 * Searches for the specified @file in possible locations for grub
 * directories.
 * On Unix this is just a wrapper around gchd_get_grub_file_from_root,
 * but on Windows it searches on all possible volumes.
 *
 * Returns: a string containing the full path to the file, which needs
 *          to be freed by the caller, or
 *          %NULL on error.
 **/
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

    /* There could be a left-over error from the past iteration.
     * Note that we free it in the beginning, because the last error
     * needs to be kept to pass on to the caller.
     */
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
  /* FIXME: is it fair to assume that on Unix the file will always be
   * in /boot/grub ?
   */
  cfg = gchd_get_grub_file_from_root (gchd, "/", file, error);
#endif

  if (cfg == NULL && error && *error == NULL)
  {
    /* We never found the directory */
    g_set_error (error, GCHD_ERROR,
                GCHD_ERROR_NO_VOLUMES,
                "No volumes were found in the system");
  }

  return cfg;
}

/**
 * gchd_set_grub_dir:
 * @gchd      : a #Gchd.
 * @grub_dir  : the string containing the grub directory to work with.
 *
 * Sets a grub directory. Usually used to avoid searching for the grub
 * directory. The parameter @grub_dir can be freed after the call.
 **/
void
gchd_set_grub_dir (Gchd * gchd, const gchar * grub_dir)
{
  g_free (gchd->grub_dir);
  gchd->grub_dir = g_strdup (grub_dir);
}

/**
 * gchd_get_grub_directory:
 * @gchd  : a #Gchd.
 *
 * Returns: the current grub directory, which may be %NULL. The string
 *          should be freed by the caller.
 **/
const gchar *
gchd_get_grub_dir (Gchd * gchd)
{
  return g_strdup (gchd->grub_dir);
}

/* interface functions */

/**
 * gchd_get_default_entry:
 * @gchd  : a #Gchd.
 * @error : error to be set if the default entry could not be determined.
 *
 * Find the default grub entry.
 *
 * Returns: a string containing the default entry if there was one,
 *          "" if there is no default entry, and
 *          %NULL if an error occurred.
 *
 *          The returned string needs to be freed.
 **/
gchar *
gchd_get_default_entry (Gchd * gchd, GError **error)
{
  g_assert (gchd->get_default_entry != NULL);
  g_assert (!error || !*error);

  return gchd->get_default_entry (gchd, error);
}

/**
 * gchd_set_default_entry:
 * @gchd     : a #Gchd.
 * @entry    : default entry to boot.
 * @callback : function to call once setting the default is done.
 * @data     : pointer to pass to the callback.
 * @error    : error to be set if the default entry could not be set.
 *
 * Sets the default grub entry. This is an asynchronous function which
 * issues a callback once setting the entry is done.
 *
 * Returns: %TRUE if the precondition for setting the default is met,
 *          %FALSE if an error occurred.
 **/
gboolean
gchd_set_default_entry (Gchd * gchd, gchar * entry, GchdSetDefaultCallback callback, gpointer data, GError **error)
{
  g_assert (gchd->set_default_entry != NULL);
  g_assert (!error || !*error);

  gchd->set_callback = callback;
  gchd->set_callback_data = data;

  return gchd->set_default_entry (gchd, entry, error);
}

/**
 * gchd_uses_default:
 * @gchd  : a #Gchd.
 * @error : error to be set if grub does not use the default entry.
 *
 * Checks if grub.cfg is set up to use the default entry set by the
 * grub environment.
 *
 * When this returns false, user action is required, and instructions on
 * what to do are returned in the error parameter.
 *
 * IMPORTANT: Requires that gchd_get_menu_entries has been called!
 *
 * Returns: %TRUE if grub is using the saved default,
 *          %FALSE if grub is not set up correctly.
 **/
gboolean
gchd_uses_default (Gchd * gchd, GError ** error)
{
#ifdef G_OS_WIN32
  const gint index = 0;
#else
  const gint index = distro;
#endif

  g_assert (index >= 0 && index < n_not_using_default_message);

  if (!gchd->menu.default_saved)
  {
    g_set_error (error,
                 GCHD_ERROR,
                 GCHD_ERROR_NOT_USING_DEFAULT,
                 "Grub is not configured to use the default set by this program.\n"
                 "\n"
                 "%s", not_using_default_message[index]);
    return FALSE;
  }

  return TRUE;
}
