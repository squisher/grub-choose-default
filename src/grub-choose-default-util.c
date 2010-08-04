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

#include "gchd-error.h"
#include "gchd-util.h"
#include "grub-choose-default-util.h"

/**
 * grub_choose_default_error:
 * #parent  : a parent window, or NULL
 * #error   : an error to display
 *
 * Displays the message of #error. A modal dialog is used if the optional
 * parameter #parent is non-%NULL.
 **/
void
grub_choose_default_error (GtkWidget *parent, GError *error)
{
  grub_choose_default_error_message (parent, error->message);
}

/**
 * grub_choose_default_error:
 * #parent  : a parent window, or NULL
 * #message : an error message to display
 *
 * Displays the message of #error. A modal dialog is used if the optional
 * parameter #parent is non-%NULL.
 **/
void
grub_choose_default_error_message (GtkWidget *parent, gchar * message)
{
  GtkWidget *dialog;

  dialog = gtk_message_dialog_new (GTK_WINDOW (parent),
                                   (parent != NULL) ? GTK_DIALOG_MODAL : 0,
                                   GTK_MESSAGE_ERROR,
                                   GTK_BUTTONS_CLOSE,
                                   "%s", message);

  g_warning ("%s", message);

  gtk_dialog_run (GTK_DIALOG (dialog));

  gtk_widget_destroy (dialog);
}

/**
 * grub_choose_default_exec:
 * #directory   : the directory to search the script in
 * #script      : the file we should try to execute
 *
 * Searches in directory for script or script* and tries to execute it.
 */
gboolean
grub_choose_default_exec(const gchar * directory, const gchar * script, gboolean sync, GError **error)
{
  gchar * path;
#ifdef G_OS_WIN32
  GDir * dir;
  const gchar * fn;
  gchar * dirname, * basename;
#endif

  g_assert (error == NULL || *error == NULL);

  path = g_build_filename (directory, script, NULL);

#ifdef G_OS_WIN32
  dirname = g_path_get_dirname (path);
  basename = g_path_get_basename (path);
  g_free (path);

  dir = g_dir_open (dirname, 0, NULL);

  if (dir == NULL)
  {
    g_set_error (error, GCHD_ERROR,
                 GCHD_ERROR_FILE_NOT_FOUND,
                 "Could not find or open %s", dirname);
    return FALSE;
  }

  while ((fn = g_dir_read_name (dir)) != NULL)
  {
    DBG ("Considering file %s", fn);
    if (g_str_has_prefix (fn, basename))
    {
      DBG (" -> has prefix %s", basename);
      path = g_build_filename (dirname, fn, NULL);

      break;
    }
  }
  g_dir_close (dir);
#endif

  g_print ("Trying to execute with prefix %s\n", path);

  if (g_file_test (path, G_FILE_TEST_IS_EXECUTABLE))
  {
    gchar *argv[3];
    gboolean r;

#ifdef G_OS_WIN32
    if (g_str_has_suffix (path, ".vbs"))
    {
      argv[0] = "cscript.exe";
      argv[1] = path;
      argv[2] = NULL;
    }
    else
#endif
    {
      argv[0] = path;
      argv[1] = NULL;
    }

    if (sync) {
      r = g_spawn_sync (NULL,
                        argv,
                        NULL,
                        G_SPAWN_SEARCH_PATH,
                        NULL,
                        NULL,
                        NULL,
                        NULL, 
                        NULL, 
                        error);
    } else {
      r = g_spawn_async (NULL,
                         argv,
                         NULL,
                         G_SPAWN_SEARCH_PATH,
                         NULL,
                         NULL,
                         NULL,
                         error);
    }

    g_free (path);
    return r;
  }

  g_free (path);
  g_set_error (error, GCHD_ERROR,
                GCHD_ERROR_FILE_NOT_FOUND,
                "Could not find a script %s in %s", script, directory);
  return FALSE;
}

