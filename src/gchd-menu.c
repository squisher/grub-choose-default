#include <stdio.h>
#include <glib.h>
#include <string.h>

#include "gchd-menu.h"
#include "gchd-error.h"
#include "gchd-util.h"

/* prototypes */

static gint parse_entries (GchdMenu *menu, gchar ** contents);

/* implementations */

static gint
parse_entries (GchdMenu *menu, gchar ** contents) {
  gchar * cp, * c, * e, *t;
  GchdEntry * entry;
  gchar quote;
  static const gchar *mi = "menuentry";
  static const gchar *sd = "set default=\"${saved_entry}\"";
  static const gchar *sm = "submenu";
  int level = 0;

  for (cp = NULL, c = *contents; *c != '\0'; cp = c, c++)
  {
    if (*c == '{')
      level++;

    if (cp == NULL || *cp == '\n') {
      /* beginning of a line */

      /* skip white spaces */
      while (*c == ' ' || *c == '\t') c++;

      /* look for the letter 'm' at the beginning of a line */
      if (*c == 'm' || *c == 's')
      {
        t = c;
        if (strncmp (c, sd, strlen(sd)) == 0)
        {
          /* Matched sd to the beginning of the line.
          * That's good enough, we assume it's only spaces
          * and comments on the remainder of the line. */

          c += strlen (sd);
          menu->default_saved = TRUE;
          continue;
        }
        else if (strncmp (c, mi, strlen(mi)) == 0)
        {
          /* matched menuentry */

          c += strlen (mi);
        }
        else if (strncmp (c, sm, strlen(sm)) == 0)
        {
          /* matched submenu */

          c += strlen (sm);
        }
        else {
          continue;
        }

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
          if (*c != '\'')
            continue;
          else
          {
            quote = '\'';
            c++;
          }
        else
        {
          quote = '"';
          c++;
        }

        /* find the (closing) quotes */
        for (e = c; (*e != '\0') && (*e != quote); e++) ;

        if (*e == '\0') {
          c = e;
          continue;
        }

        /* copy the entry */
        *e = '\0';
        entry = g_new0 (GchdEntry, 1);
        entry->name = g_strdup (c);
        c = e;

        /* level should always be zero */
        DBG ("%s at level %d", entry->name, level);

        menu->entries = g_list_prepend (menu->entries, entry);
        menu->n_entries++;

        if (*t == 's') {
          /* submenu */
          int n;

          /* find the opening bracket,
           * TODO: assumes no brackets in the string! */
          /* move past the \0 that was added above */
          c++;
          while (*c != '{' && *c != '\0') c++;
          if (*c == '\0')
            return -1;
          /* move past the { */
          c++;

          GchdMenu * sub;
          sub = g_new0(GchdMenu, 1);
          sub->name = g_strdup (entry->name);
          sub->parent = menu;
          entry->submenu = sub;

          DBG ("submenu %s", sub->name);
          n = parse_entries (sub, &c);
          if (n < 0)
            return n;
        }
      }
    }

    if (*c == '}')
    {
      level--;
      if (level < 0)
      {
        DBG ("Returning from submenu");

        entry = g_new0 (GchdEntry, 1);
        entry->name = g_strdup ("<-");
        entry->parentmenu = menu->parent;

        menu->entries = g_list_prepend (menu->entries, entry);
        menu->n_entries++;

        c++;
        break;
      }
    }
  }

  menu->entries = g_list_reverse (menu->entries);
  *contents = c;

  return menu->n_entries;
}

/* public */

gboolean
gchd_get_menu (Gchd *gchd, GError **error) {
  const gchar * cfg;
  gchar * contents, * c;
  gsize len;
  gboolean r;

  g_assert (!error || !(*error));
  
  cfg = gchd_get_grub_file (gchd, "grub.cfg", error);

  if (!cfg) {
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

  c = contents;
  parse_entries (&(gchd->menu), &c);
  if (*c != '\0')
    g_warning ("Menu parsing left '%s' over...", c);

  g_free (contents);

  return TRUE;
}

void
gchd_entry_free (gpointer data, gpointer user_data)
{
  g_free (((GchdEntry *)data)->name);
  g_free (data);
}

void
gchd_menu_free (GchdMenu *menu)
{
  g_free (menu->loc);
  g_list_foreach (menu->entries, gchd_entry_free, NULL);
  g_list_free (menu->entries);
}
