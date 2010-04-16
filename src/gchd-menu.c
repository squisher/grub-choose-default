#include <stdio.h>
#include <glib.h>
#include <string.h>

#include "gchd-menu.h"
#include "gchd-error.h"

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
gchd_get_menu (Gchd *gchd, GError **error) {
  const gchar * cfg;
  gchar * contents;
  gsize len;
  gboolean r;

  g_assert (!error || !(*error));
  
  cfg = gchd_get_grub_file (gchd, "grub.cfg", error);

  if (!cfg || !*cfg) {
    return FALSE;
  }

  r = g_file_get_contents (cfg, &contents, &len, error);

  if (!r) {
    g_set_error (error, GCHD_ERROR,
                 GCHD_ERROR_FILE_NOT_READABLE,
                 "Failed to read contents of %s", cfg);
    return FALSE;
  }

  gchd->menu.loc = g_strdup (cfg);

  parse_entries (&(gchd->menu), contents);

  g_free (contents);

  return TRUE;
}

void
gchd_menu_free (GchdMenu *menu)
{
  g_free (menu->loc);
  g_list_foreach (menu->entries, (GFunc) g_free, NULL);
  g_list_free (menu->entries);
}
