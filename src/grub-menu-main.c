#include <glib.h>
#include <stdlib.h>
#include "grub-menu.h"

int
main (int argc, char **argv) {
  GrubMenu * gm;

  gm = grub_menu_get (NULL);

  if (gm == NULL) {
    g_print ("Could not open the grub configuration!\n");
    return EXIT_FAILURE;
  } 
  else
  {
    GList * entries;

    for (entries = gm->entries; entries != NULL; entries = entries->next) {
      g_print ("%s\n", (gchar *) entries->data);
    }

    grub_menu_free (&gm);

    return EXIT_SUCCESS;
  }
}
