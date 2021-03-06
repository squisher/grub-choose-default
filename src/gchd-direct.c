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

#ifdef HAVE_STRING_H 
#  include <string.h>
#endif

#include <glib/gstdio.h>

#include "gchd-direct.h"
#include "grub-envblk.h"
#include "gchd-error.h"
#include "gchd-util.h"

static const gchar * default_key = "saved_entry";
static const gchar * prev_default_key = "prev_saved_entry";
static gchar * current_default_entry = NULL;

typedef struct {
  gchar * contents;
  grub_envblk_t env;
  gboolean set_result;
} GchdDirectPrivate;

#define GCHD_DIRECT_PRIVATE(x) ((GchdDirectPrivate *)(x))


/* prototypes */

static gchar * get_default_entry (Gchd * gchd, GError **error);
static gboolean set_default_entry (Gchd * gchd, gchar * entry, GError **error);
static int find (const char *name, const char *value);
static gboolean signal_set_done (gpointer data);


/* implementations */

void
gchd_direct_init (Gchd * gchd)
{
  g_print ("Using direct method to set and get default entries\n");

  gchd->get_default_entry = get_default_entry;
  gchd->set_default_entry = set_default_entry;

  gchd->data = (gpointer) g_new0 (GchdDirectPrivate, 1);
}

/* this method is NOT thread safe */
static gchar *
get_default_entry (Gchd * gchd, GError **error)
{
  GchdDirectPrivate *priv = GCHD_DIRECT_PRIVATE (gchd->data);

  gboolean r;
  gsize len;
  gchar * env_filename;

  env_filename = gchd_get_grub_file (gchd, "grubenv", error);

  if (env_filename == NULL) {
    /* we did not find grubenv, so assume that no default is set */
    if (*error)
    {
      g_error_free (*error);
      *error = NULL;
    }

    return "";
  }

  g_print ("Operating on %s\n", env_filename);

  r = g_file_get_contents (env_filename, &(priv->contents), &len, error);

  if (!r)
  {
    return NULL;
  }

  priv->env = grub_envblk_open (priv->contents, len);

  if (priv->env == NULL)
  {
    g_critical ("Unable to parse the grub environment");

    return NULL;
  }

  grub_envblk_iterate (priv->env, find);

  if (current_default_entry == NULL)
  {
    g_warning ("Could not find the default entry");
  }

  return current_default_entry;
}

static gboolean
set_default_entry (Gchd * gchd, gchar * entry, GError **error)
{
  GchdDirectPrivate *priv = GCHD_DIRECT_PRIVATE (gchd->data);

  gint r;
  gboolean b;
  gchar * env_filename;

  r = 0;
  env_filename = gchd_get_grub_file (gchd, "grubenv", error);

  if (env_filename)
  {
    r = grub_envblk_set (priv->env, default_key, entry);
    if (gchd->once && r)
    {
      r = grub_envblk_set (priv->env, prev_default_key, current_default_entry);
    }
  }

  if (!r)
  {
    g_set_error (error,
                 GCHD_ERROR, GCHD_ERROR_FAILED_SETTING_ENTRY,
                 "Failed to set entry");
    return FALSE;
  }

  b = g_file_set_contents (env_filename,
                           grub_envblk_buffer (priv->env),
                           grub_envblk_size (priv->env),
                           error);

  if (!b)
  {
    g_set_error (error,
                 GCHD_ERROR, GCHD_ERROR_FAILED_WRITING_ENV,
                 "Failed to write env");
    return FALSE;
  }

  /* an idle function makes sure the callback is issued strictly after this
   * function returns */
  g_idle_add (signal_set_done, gchd);

  return TRUE;
}

static gboolean
signal_set_done (gpointer data)
{
  Gchd * gchd = (Gchd *) data;

  g_assert (gchd != NULL);

  /* this function is only called when setting the default
   * succeeded, so we always signal TRUE here */
  gchd->set_callback (gchd, TRUE, NULL, gchd->set_callback_data);

  return FALSE;
}

static int
find (const char *name, const char *value)
{
  if (strcmp (name, default_key) == 0)
  {
    current_default_entry = g_strdup (value);
    return TRUE;
  }
  return FALSE; 
}
