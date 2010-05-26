#include <glib.h>
#include <stdlib.h>

#include "gchd.h"

void
handle_error (Gchd *gchd, GError *error)
{
  g_print ("Error: %s\n", error->message);
  g_error_free (error);
  gchd_free (gchd);
  exit (EXIT_FAILURE);
}

int
main (int argc, char **argv) {
  Gchd *gchd;
  GError *error = NULL;
  gint n_entries;
  GList * entries;
  gchar *def_entry;
  gboolean r;


  gchd = gchd_new ();

  n_entries = gchd_get_menu_entries (gchd, &entries, &error);

  if (n_entries == -1)
  {
    handle_error (gchd, error);
  } 

  for (; entries != NULL; entries = entries->next) {
    g_print ("%s\n", (gchar *) entries->data);
  }

  def_entry = gchd_get_default_entry (gchd, &error);

  if (def_entry == NULL)
  {
    handle_error (gchd, error);
  }

  g_print ("\nDefault entry: %s\n", def_entry);

#if 0
  /* right now gchd-test doesn't use a main loop, so
   * let's forget about the async call for now */
  if (argc > 1)
  {
    gchar * entry = argv[1];
    r = gchd_set_default_entry (gchd, entry, &error);

    if (!r)
    {
      handle_error (gchd, error);
    }

    g_print ("Set default entry to: %s\n", entry);
  }
  else
  {
    g_print ("Not setting default entry, none specified\n");
  }
#endif

  gchd_free (gchd);

  return  EXIT_SUCCESS;
}
