#ifndef _HAVE_GRUB_MENU
#define _HAVE_GRUB_MENU

#include <glib.h>

G_BEGIN_DECLS

typedef struct {
  gchar * loc;
  GList * entries; /* of type char* */
} GrubMenu;


void grub_menu_free (GrubMenu ** gm);
GrubMenu * grub_menu_get (void);

G_END_DECLS

#endif /* _HAVE_GRUB_MENU */
