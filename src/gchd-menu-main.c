#include <glib.h>
#include <stdlib.h>
#include "gchd.h"

int
main (int argc, char **argv) {
  Gchd *gchd;
  GError *error = NULL;
  gint n_entries;
  GList * entries;
  gint ret;


  gchd = gchd_new ();

  n_entries = gchd_get_menu_entries (gchd, &entries, &error);

  if (n_entries == -1)
  {
    g_print ("Error: %s", error->message);
    g_error_free (error);
    ret = EXIT_FAILURE;
  } 
  else
  {
    for (; entries != NULL; entries = entries->next) {
      g_print ("%s\n", (gchar *) entries->data);
    }


    ret = EXIT_SUCCESS;
  }

  gchd_free (gchd);

  return ret;
}
