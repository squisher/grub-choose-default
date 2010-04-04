#include <stdio.h>
#include <glib.h>
#include <string.h>

#include "gchd-menu.h"
#include "gchd-error.h"

static const gchar * grub_config_locations[] = {
  "/boot/grub/grub.cfg",
  "/grub/grub.cfg",
  NULL
};

/* prototypes */

static gint parse_entries (GchdMenu *menu, gchar * contents);

/* implementations */

static gint
parse_entries (GchdMenu *menu, gchar * contents) {
  gchar * cp, * c, * e;
  gchar * entry;
  static const gchar *mi = "menuentry";

  for (cp = NULL, c = contents; *c != '\0'; cp = c, c++) {
    /* look for the letter 'm' at the beginning of a line */
    if (*c == 'm' && (cp == NULL || *cp == '\n')) {
      if (strncmp (c, mi, strlen(mi)) == 0) {
        c += strlen (mi);
        /* we expect at least one white space */

        if (*c != ' ')
          continue;
        else
          c++;
        
        /* skip additional white spaces */
        while (*c == ' ')
          c++;

        /* expect the (opening) quotes */
        if (*c != '"')
          continue;
        else
          c++;

        /* find the (closing) quotes */
        for (e = c; (*e != '\0') && (*e != '"'); e++) ;

        if (*e == '\0') {
          c = e;
          continue;
        }

        /* copy the entry */
        *e = '\0';
        entry = g_strdup (c);
        c = e;

        menu->entries = g_list_prepend (menu->entries, entry);
        menu->n_entries++;
      }
    }
  }

  menu->entries = g_list_reverse (menu->entries);

  return menu->n_entries;
}

/* public */

gboolean
gchd_get_menu (GchdMenu *gm, GError **error) {
  const gchar ** cfg = NULL;
  gchar * contents;
  gsize len;
  gboolean r;

  g_assert (!error || !(*error));

  for (cfg = grub_config_locations; *cfg != NULL; cfg++) {
    r = g_file_test (*cfg, G_FILE_TEST_EXISTS);

    if (!r) {
      continue;
    } else {
      break;
    }
  }

  if (!cfg || !*cfg) {
    g_set_error (error,
                 GCHD_ERROR,
                 GCHD_ERROR_CFG_NOT_FOUND,
                 "Failed to find grub.cfg");
    return FALSE;
  }

  r = g_file_get_contents (*cfg, &contents, &len, error);

  if (!r) {
    g_set_error (error,
                 GCHD_ERROR,
                 GCHD_ERROR_CFG_NOT_READABLE,
                 "Failed to read contents of grub.cfg");
    return FALSE;
  }

  gm->loc = g_strdup (*cfg);

  parse_entries (gm, contents);

  g_free (contents);

  return TRUE;
}
