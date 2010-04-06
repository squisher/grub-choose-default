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

#include "grub-choose-default-widget.h"

static void
grub_choose_default_widget_init (gpointer g_iface)
{
  GrubChooseDefaultWidgetInterface * iface = g_iface;
  static gboolean is_initialized = FALSE;

  if (!is_initialized)
  {
    g_object_interface_install_property (g_iface,
                                         g_param_spec_string ("default-entry",
                                                              "Default Entry",
                                                              "Default grub boot entry",
                                                              NULL,
                                                              G_PARAM_READWRITE));
    g_object_interface_install_property (g_iface,
                                         g_param_spec_boolean ("auto-commit",
                                                              "Auto-commit",
                                                              "Auto-commit selections",
                                                              FALSE,
                                                              G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

    iface->signals[GRUB_CHOOSE_DEFAULT_WIDGET_SIGNAL_SELECTED] = g_signal_new ("selected",
                                                                               G_TYPE_FROM_CLASS (g_iface), G_SIGNAL_ACTION,
                                                                               G_STRUCT_OFFSET (GrubChooseDefaultWidgetInterface, selected),
                                                                               NULL, NULL, g_cclosure_marshal_VOID__STRING,
                                                                               G_TYPE_NONE, 1, G_TYPE_STRING);

    is_initialized = TRUE;
  }
}

GType
grub_choose_default_widget_get_type (void)
{
  static GType iface_type = 0;

  if (iface_type == 0)
  {
    static const GTypeInfo info = {
      sizeof (GrubChooseDefaultWidgetInterface),
      grub_choose_default_widget_init,
      NULL,
    };

    iface_type = g_type_register_static (G_TYPE_INTERFACE, "GrubChooseDefaultWidget",
                                         &info, 0);
  }

  return iface_type;
}

/* public */

gboolean
grub_choose_default_widget_commit (GrubChooseDefaultWidget * widget, GError **error)
{
  g_return_val_if_fail (GRUB_CHOOSE_DEFAULT_IS_WIDGET (widget), FALSE);

  return GRUB_CHOOSE_DEFAULT_WIDGET_GET_INTERFACE (widget)->commit (widget, error);
}
