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

#ifndef __GRUB_CHOOSE_DEFAULT_WINDOW__
#define __GRUB_CHOOSE_DEFAULT_WINDOW__

#include <gtk/gtk.h>


G_BEGIN_DECLS

#define GRUB_CHOOSE_DEFAULT_TYPE_WINDOW grub_choose_default_window_get_type()

#define GRUB_CHOOSE_DEFAULT_WINDOW(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), GRUB_CHOOSE_DEFAULT_TYPE_WINDOW, GrubChooseDefaultWindow))

#define GRUB_CHOOSE_DEFAULT_WINDOW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), GRUB_CHOOSE_DEFAULT_TYPE_WINDOW, GrubChooseDefaultWindowClass))

#define GRUB_CHOOSE_DEFAULT_IS_WINDOW(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GRUB_CHOOSE_DEFAULT_TYPE_WINDOW))

#define GRUB_CHOOSE_DEFAULT_IS_WINDOW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), GRUB_CHOOSE_DEFAULT_TYPE_WINDOW))

#define GRUB_CHOOSE_DEFAULT_WINDOW_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), GRUB_CHOOSE_DEFAULT_TYPE_WINDOW, GrubChooseDefaultWindowClass))

typedef struct {
  GtkDialog parent;
} GrubChooseDefaultWindow;

typedef struct {
  GtkDialogClass parent_class;
} GrubChooseDefaultWindowClass;

GType grub_choose_default_window_get_type (void);

GrubChooseDefaultWindow* grub_choose_default_window_new (void);

G_END_DECLS

#endif /* __GRUB_CHOOSE_DEFAULT_WINDOW__ */
