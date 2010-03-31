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

#include <gtk/gtk.h>

#include "grub-choose-default-button-box.h"
#include "grub-menu.h"

/*- private prototypes -*/

/*- globals -*/

enum {
  PROP_0,
};


/*****************/
/*- class setup -*/
/*****************/

G_DEFINE_TYPE (GrubChooseDefaultButtonBox, grub_choose_default_button_box, GTK_TYPE_VBOX)

#define GET_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), GRUB_CHOOSE_DEFAULT_TYPE_BUTTON_BOX, GrubChooseDefaultButtonBoxPrivate))

typedef struct _GrubChooseDefaultButtonBoxPrivate GrubChooseDefaultButtonBoxPrivate;

struct _GrubChooseDefaultButtonBoxPrivate {
  GrubMenu * menu;
};

static void
grub_choose_default_button_box_get_property (GObject *object, guint property_id,
                              GValue *value, GParamSpec *pspec)
{
  //GrubChooseDefaultButtonBoxPrivate *priv = GET_PRIVATE (GRUB_CHOOSE_DEFAULT_BUTTON_BOX (object));

  switch (property_id) {
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
  }
}

static void
grub_choose_default_button_box_set_property (GObject *object, guint property_id,
                              const GValue *value, GParamSpec *pspec)
{
  //GrubChooseDefaultButtonBoxPrivate *priv = GET_PRIVATE (GRUB_CHOOSE_DEFAULT_BUTTON_BOX (object));

  switch (property_id) {
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
  }
}

static void
grub_choose_default_button_box_finalize (GObject *object)
{
  GrubChooseDefaultButtonBoxPrivate *priv = GET_PRIVATE (GRUB_CHOOSE_DEFAULT_BUTTON_BOX (object));

  grub_menu_free (&(priv->menu));
  G_OBJECT_CLASS (grub_choose_default_button_box_parent_class)->finalize (object);
}

static void
grub_choose_default_button_box_class_init (GrubChooseDefaultButtonBoxClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (GrubChooseDefaultButtonBoxPrivate));

  object_class->get_property = grub_choose_default_button_box_get_property;
  object_class->set_property = grub_choose_default_button_box_set_property;
  object_class->finalize = grub_choose_default_button_box_finalize;
}

static void
grub_choose_default_button_box_init (GrubChooseDefaultButtonBox *self)
{
  GrubChooseDefaultButtonBoxPrivate *priv = GET_PRIVATE (GRUB_CHOOSE_DEFAULT_BUTTON_BOX (self));
  GError *error = NULL;

  priv->menu = grub_menu_get (&error);

  if (error) {
    g_warning (error->message);
    g_error_free (error);

    return;
  }
}


/***************/
/*- internals -*/
/***************/

/*******************/
/*- public methods-*/
/*******************/

GrubChooseDefaultButtonBox*
grub_choose_default_button_box_new (void)
{
  return g_object_new (GRUB_CHOOSE_DEFAULT_TYPE_BUTTON_BOX, NULL);
}
