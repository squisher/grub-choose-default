/*
 * Copyright (c) 2010     David Mohr <david@mcbf.net>
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
#ifndef _HAVE_GCHD_MESSAGES_H
#define _HAVE_GCHD_MESSAGES_H

#include <glib.h>

static const gchar * not_using_default_message [] =
{
  /* Generic message to be displayed while in Windows
   * and we have no clue what distro is in use.
   * (0)
   */
  "Usually it is required to change the grub configuration under Linux.",

  /* Generic Linux message, the default
   * (1)
   */
  "Set 'set default=\"${saved_entry}\"' in grub.cfg,\n"
  "but since grub.cfg is generated, the exact way to save\n"
  "this setting is distribution specific.",

  /* For Debian and Ubuntu at least
   * (2)
   */
  "As root edit '/etc/default/grub' and set 'GRUB_DEFAULT=saved'.\n"
  "Then run 'update-grub'.",
};

static const gint n_not_using_default_message = 3;
static const gint distro = DISTRIBUTION;

#endif /* _HAVE_GCHD_MESSAGES_H */
