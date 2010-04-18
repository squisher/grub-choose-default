/*
 *  Copyright (C) 2010 David Mohr <david@mcbf.net>
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
 *  
 */

#ifndef __GRUB_CHOOSE_DEFAULT_BUTTON_BOX__
#define __GRUB_CHOOSE_DEFAULT_BUTTON_BOX__

#include <glib-object.h>

G_BEGIN_DECLS

#define GRUB_CHOOSE_DEFAULT_TYPE_BUTTON_BOX grub_choose_default_button_box_get_type()

#define GRUB_CHOOSE_DEFAULT_BUTTON_BOX(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), GRUB_CHOOSE_DEFAULT_TYPE_BUTTON_BOX, GrubChooseDefaultButtonBox))

#define GRUB_CHOOSE_DEFAULT_BUTTON_BOX_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), GRUB_CHOOSE_DEFAULT_TYPE_BUTTON_BOX, GrubChooseDefaultButtonBoxClass))

#define GRUB_CHOOSE_DEFAULT_IS_BUTTON_BOX(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GRUB_CHOOSE_DEFAULT_TYPE_BUTTON_BOX))

#define GRUB_CHOOSE_DEFAULT_IS_BUTTON_BOX_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), GRUB_CHOOSE_DEFAULT_TYPE_BUTTON_BOX))

#define GRUB_CHOOSE_DEFAULT_BUTTON_BOX_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), GRUB_CHOOSE_DEFAULT_TYPE_BUTTON_BOX, GrubChooseDefaultButtonBoxClass))

typedef struct {
  GtkVBox parent;
} GrubChooseDefaultButtonBox;

typedef struct {
  GtkVBoxClass parent_class;
} GrubChooseDefaultButtonBoxClass;

GType grub_choose_default_button_box_get_type (void);

GrubChooseDefaultButtonBox* grub_choose_default_button_box_new (gchar * grub_dir);

G_END_DECLS

#endif /* __GRUB_CHOOSE_DEFAULT_BUTTON_BOX__ */
