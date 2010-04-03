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

#include "grub-choose-default-window.h"
#include "grub-choose-default-button-box.h"

/*- private prototypes -*/

/*- globals -*/

enum {
  PROP_0,
};


/*****************/
/*- class setup -*/
/*****************/

G_DEFINE_TYPE (GrubChooseDefaultWindow, grub_choose_default_window, GTK_TYPE_DIALOG)

#define GET_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), GRUB_CHOOSE_DEFAULT_TYPE_WINDOW, GrubChooseDefaultWindowPrivate))

typedef struct _GrubChooseDefaultWindowPrivate GrubChooseDefaultWindowPrivate;

struct _GrubChooseDefaultWindowPrivate {
  GrubChooseDefaultButtonBox * box;
};

static void
grub_choose_default_window_get_property (GObject *object, guint property_id,
                              GValue *value, GParamSpec *pspec)
{
  //GrubChooseDefaultWindowPrivate *priv = GET_PRIVATE (GRUB_CHOOSE_DEFAULT_WINDOW (object));

  switch (property_id) {
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
  }
}

static void
grub_choose_default_window_set_property (GObject *object, guint property_id,
                              const GValue *value, GParamSpec *pspec)
{
  //GrubChooseDefaultWindowPrivate *priv = GET_PRIVATE (GRUB_CHOOSE_DEFAULT_WINDOW (object));

  switch (property_id) {
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
  }
}

static void
grub_choose_default_window_finalize (GObject *object)
{
  G_OBJECT_CLASS (grub_choose_default_window_parent_class)->finalize (object);
}

static void
grub_choose_default_window_class_init (GrubChooseDefaultWindowClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (GrubChooseDefaultWindowPrivate));

  object_class->get_property = grub_choose_default_window_get_property;
  object_class->set_property = grub_choose_default_window_set_property;
  object_class->finalize = grub_choose_default_window_finalize;
}

static void
grub_choose_default_window_init (GrubChooseDefaultWindow *self)
{
  GrubChooseDefaultWindowPrivate *priv = GET_PRIVATE (GRUB_CHOOSE_DEFAULT_WINDOW (self));
  GtkWidget *area, *scrolled;
  GtkRequisition req;

  area = gtk_dialog_get_content_area (GTK_DIALOG (self));

  scrolled = gtk_scrolled_window_new (NULL, NULL);
  gtk_container_add (GTK_CONTAINER (area), scrolled);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_widget_show (scrolled);

  priv->box = grub_choose_default_button_box_new ();

  gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scrolled), GTK_WIDGET (priv->box));
  gtk_widget_show (GTK_WIDGET (priv->box));

  gtk_widget_size_request (GTK_WIDGET (priv->box), &req);
  req.height += 50;
  req.width += 50;

  if (req.width > 600 )
    req.width = 600;
  if (req.height > 800)
    req.height = 800;

  g_debug ("Will request size %d by %d", req.width, req.height);

  gtk_window_set_default_size (GTK_WINDOW (self), req.width, req.height);
}


/***************/
/*- internals -*/
/***************/

/*******************/
/*- public methods-*/
/*******************/

GrubChooseDefaultWindow*
grub_choose_default_window_new (void)
{
  return g_object_new (GRUB_CHOOSE_DEFAULT_TYPE_WINDOW, NULL);
}
