#ifndef _HAVE_GCD_ERROR
#define _HAVE_GCD_ERROR

#include <glib.h>

G_BEGIN_DECLS

typedef enum
{
  GRUB_CHOOSE_DEFAULT_ERROR_NONE,
  GRUB_CHOOSE_DEFAULT_ERROR_CFG_NOT_FOUND,
} GrubChooseDefaultError;

#define GRUB_CHOOSE_DEFAULT_ERROR grub_choose_default_error_quark ()

GQuark grub_choose_default_error_quark (void);

G_END_DECLS

#endif /* _HAVE_GCD_ERROR */
