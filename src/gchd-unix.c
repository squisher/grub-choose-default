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

#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#include "gchd-unix.h"
#include "gchd-util.h"

static const gchar * default_key = "saved_entry";


/* prototypes */

static gchar * get_default_entry (Gchd * gchd, GError **error);
static gboolean set_default_entry (Gchd * gchd, gchar * entry, GError **error);


/* implementations */

void
gchd_unix_init (Gchd * gchd)
{
  g_print ("Using unix utilities to set and get default entries\n");

  gchd->get_default_entry = get_default_entry;
  gchd->set_default_entry = set_default_entry;
}

static gchar *
get_default_entry (Gchd * gchd, GError **error)
{
  /* Essentially run this pipeline:
   * grub-editenv list | grep saved_entry | cut -d'=' -f2
   */
  gchar * argv [3];
  gchar * s_output;
  gchar * s_error;
  gint exit_status;
  gboolean r;
  gchar **lines;
  gchar **line;
  gchar * default_entry = NULL;

  argv[0] = "grub-editenv";
  argv[1] = "list";
  argv[2] = NULL;

  r = g_spawn_sync (NULL,
                argv,
                NULL,
                G_SPAWN_SEARCH_PATH,
                NULL,
                NULL,
                &s_output,
                &s_error,
                &exit_status,
                error);

  if (!r)
  {
    /* an error occurred which is passed onto the caller through
     * the error argument */
    return NULL;
  }

  lines = g_strsplit (s_output, "\n", -1);
  for (line = lines; line != NULL && (*line) != NULL; line++)
  {
    gchar * value;
    gint default_key_len = strlen (default_key);

    if (strncmp (*line, default_key, default_key_len) == 0)
    {
      value = (*line) + default_key_len + 1;
      default_entry = g_strdup (value);
      break;
    }
  }

  return default_entry;
}

static gboolean
set_default_entry (Gchd * gchd, gchar * entry, GError **error)
{
  /* Execute:
   * grub-set-default $entry
   */
  gchar * argv [4];
  gchar * s_output;
  gchar * s_error;
  gint exit_status;
  gboolean r;
  gint i;

  g_assert (entry != NULL);

  if (geteuid() != 0)
  {
    argv[0] = "sudo";
    i = 1;
    g_print ("Using sudo to set default entry\n");
  }
  else
  {
    i = 0;
  }

  argv[i++] = "grub-set-default";
  argv[i++] = entry;
  argv[i++] = NULL;

  r = g_spawn_sync (NULL,
                argv,
                NULL,
                G_SPAWN_SEARCH_PATH,
                NULL,
                NULL,
                &s_output,
                &s_error,
                &exit_status,
                error);

  if (!r)
  {
    /* an error occurred which is passed onto the caller through
     * the error argument */
    return FALSE;
  }

  return TRUE;
}
