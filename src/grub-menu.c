#include <stdio.h>
#include <glib.h>
#include <string.h>
#include "grub-menu.h"

static const gchar * grub_config_locations[] = {
  "/boot/grub/grub.cfg",
  "/grub/grub.cfg",
  NULL
};

GList *
parse_entries (gchar * contents) {
  GList * list = NULL;
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

        list = g_list_prepend (list, entry);
      }
    }
  }

  return g_list_reverse (list);
}

GrubMenu *
grub_menu_get () {
  GrubMenu * gm = NULL;
  const gchar ** cfg = NULL;
  gchar * contents;
  gsize len;
  GError * error = NULL;
  gboolean r;

  for (cfg = grub_config_locations; *cfg != NULL; cfg++) {
    r = g_file_test (*cfg, G_FILE_TEST_EXISTS);

    if (!r) {
      continue;
    } else {
      break;
    }
  }

  if (!cfg || !*cfg)
    return NULL;

  r = g_file_get_contents (*cfg, &contents, &len, &error);

  if (!r) {
    g_warning ("Error while getting contents of %s: %s", *cfg, error->message);
    g_error_free (error);
    return NULL;
  }

  gm = g_new0 (GrubMenu, 1);

  gm->loc = g_strdup (*cfg);

  gm->entries = parse_entries (contents);

  g_free (contents);

  return gm;
}

void
grub_menu_free (GrubMenu ** gm) {
  if (*gm != NULL) {
    g_free ((*gm)->loc);
    g_list_foreach ((*gm)->entries, (GFunc) g_free, NULL);
    g_list_free ((*gm)->entries);

    g_free (*gm);
  }
}
