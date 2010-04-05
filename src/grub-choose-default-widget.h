/*
 * Copyright (c) 2009     David Mohr <david@mcbf.net>
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

#ifndef _HAVE_GRUB_CHOOSE_DEFAULT_WIDGET
#define _HAVE_GRUB_CHOOSE_DEFAULT_WIDGET

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define GRUB_CHOOSE_DEFAULT_TYPE_WIDGET         (grub_choose_default_widget_get_type ())
#define GRUB_CHOOSE_DEFAULT_WIDGET(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), GRUB_CHOOSE_DEFAULT_TYPE_WIDGET, GrubChooseDefaultWidget))
#define GRUB_CHOOSE_DEFAULT_IS_WIDGET(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), GRUB_CHOOSE_DEFAULT_TYPE_WIDGET))
#define GRUB_CHOOSE_DEFAULT_WIDGET_GET_INTERFACE(o) (G_TYPE_INSTANCE_GET_INTERFACE ((o), GRUB_CHOOSE_DEFAULT_TYPE_WIDGET, GrubChooseDefaultWidgetInterface))

enum {
  GRUB_CHOOSE_DEFAULT_WIDGET_SIGNAL_SELECTED,
  GRUB_CHOOSE_DEFAULT_WIDGET_LAST_SIGNAL,
}; 

typedef struct {} GrubChooseDefaultWidget; /* dummy struct */

typedef struct
{
  GTypeInterface parent;

  guint signals[GRUB_CHOOSE_DEFAULT_WIDGET_LAST_SIGNAL];

  /* public functions */
  gboolean (*commit) (GrubChooseDefaultWidget * widget, GError **error);

  /* signals */
  void (*selected) (gchar * entry);
} GrubChooseDefaultWidgetInterface;

GType grub_choose_default_widget_get_type (void);
gboolean grub_choose_default_widget_commit (GrubChooseDefaultWidget * widget, GError **error);

G_END_DECLS

#endif /* _HAVE_GRUB_CHOOSE_DEFAULT_WIDGET */
