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

#include <string.h>

#include "grub-choose-default-button-box.h"
#include "gchd.h"
#include "grub-choose-default-widget.h"
#include "grub-choose-default-util.h"

/*- private prototypes -*/

static void grub_choose_default_widget_interface_init (GrubChooseDefaultWidgetInterface *iface);

static gboolean commit (GrubChooseDefaultWidget * widget, GError **error);

static void button_clicked (GtkButton *button, gpointer user_data);

/*- globals -*/

enum {
  PROP_0,
  PROP_DEFAULT_ENTRY,
  PROP_AUTO_COMMIT,
};

enum {
  LAST_SIGNAL,
}; 


/*****************/
/*- class setup -*/
/*****************/

G_DEFINE_TYPE_WITH_CODE  (GrubChooseDefaultButtonBox, grub_choose_default_button_box, GTK_TYPE_VBOX,
                          G_IMPLEMENT_INTERFACE (GRUB_CHOOSE_DEFAULT_TYPE_WIDGET, grub_choose_default_widget_interface_init));

#define GET_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), GRUB_CHOOSE_DEFAULT_TYPE_BUTTON_BOX, GrubChooseDefaultButtonBoxPrivate))

typedef struct _GrubChooseDefaultButtonBoxPrivate GrubChooseDefaultButtonBoxPrivate;

//static guint signals[LAST_SIGNAL];

struct _GrubChooseDefaultButtonBoxPrivate {
  Gchd *gchd;
  GtkWidget ** buttons;
  gchar * def_entry;
  gboolean autocommit;
};

static void
grub_choose_default_button_box_get_property (GObject *object, guint property_id,
                              GValue *value, GParamSpec *pspec)
{
  GrubChooseDefaultButtonBoxPrivate *priv = GET_PRIVATE (GRUB_CHOOSE_DEFAULT_BUTTON_BOX (object));

  switch (property_id) {
  case PROP_DEFAULT_ENTRY:
    g_value_set_string (value, priv->def_entry);
    break;
  case PROP_AUTO_COMMIT:
    g_value_set_boolean (value, priv->autocommit);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
  }
}

static void
grub_choose_default_button_box_set_property (GObject *object, guint property_id,
                              const GValue *value, GParamSpec *pspec)
{
  GrubChooseDefaultButtonBoxPrivate *priv = GET_PRIVATE (GRUB_CHOOSE_DEFAULT_BUTTON_BOX (object));

  switch (property_id) {
  case PROP_DEFAULT_ENTRY:
    priv->def_entry = g_value_dup_string (value);
    break;
  case PROP_AUTO_COMMIT:
    priv->autocommit = g_value_get_boolean (value);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
  }
}

static void
grub_choose_default_button_box_finalize (GObject *object)
{
  GrubChooseDefaultButtonBoxPrivate *priv = GET_PRIVATE (GRUB_CHOOSE_DEFAULT_BUTTON_BOX (object));

  gchd_free (priv->gchd);
  g_free (priv->def_entry);

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

  g_object_class_override_property (object_class, PROP_DEFAULT_ENTRY, "default-entry");
  g_object_class_override_property (object_class, PROP_AUTO_COMMIT, "auto-commit");

}

static void
grub_choose_default_widget_interface_init (GrubChooseDefaultWidgetInterface *iface)
{
  iface->commit = commit;
}

static void
grub_choose_default_button_box_init (GrubChooseDefaultButtonBox *self)
{
  GrubChooseDefaultButtonBoxPrivate *priv = GET_PRIVATE (GRUB_CHOOSE_DEFAULT_BUTTON_BOX (self));
  GError *error = NULL;
  int i;
  GList *entries;
  gint n_entries;
  gchar * def_entry;

  priv->gchd = gchd_new ();

  n_entries = gchd_get_menu_entries (priv->gchd, &entries, &error);

  if (n_entries == -1)
  {
    grub_choose_default_error (gtk_widget_get_toplevel (GTK_WIDGET (self)),
                               error);
    g_error_free (error);

    return;
  }

  def_entry = gchd_get_default_entry (priv->gchd, &error);

  if (def_entry == NULL)
  {
    grub_choose_default_error (gtk_widget_get_toplevel (GTK_WIDGET (self)),
                                error);
    g_error_free (error);

    return;
  }

  priv->buttons = g_new (GtkWidget *, n_entries);

  for (i=0;
       i<n_entries && entries != NULL;
       i++, entries = g_list_next (entries))
  {
    GtkWidget * button;
    gchar * entry = entries->data;
    GtkWidget * label;

    priv->buttons[i] = button = gtk_button_new ();
    g_object_set_data (G_OBJECT (button), "entry", entry);

    label = gtk_label_new ("");

    if (strcmp (entry, def_entry) == 0)
    {
      //gtk_button_set_relief (GTK_BUTTON (button), GTK_RELIEF_NORMAL);
      //gtk_label_set_attributes (GTK_LABEL (label), "use-underline");
      gchar * markup;

      markup = g_strdup_printf ("<b>%s</b>", entry);
      gtk_label_set_markup (GTK_LABEL (label), markup);
      g_free (markup);
    }
    else
    {
      gtk_label_set_text (GTK_LABEL (label), entry);
    }

    gtk_container_add (GTK_CONTAINER (button), label);
    gtk_widget_show (label);

    gtk_button_set_alignment (GTK_BUTTON (button), 0.0, 0.5);

    g_signal_connect (button, "clicked", G_CALLBACK (button_clicked), self);

    gtk_box_pack_start (GTK_BOX (self), button,
                        FALSE, FALSE, 0);
      
    gtk_widget_show (button);
  }
}


/***************/
/*- internals -*/
/***************/

static void
button_clicked (GtkButton *button, gpointer user_data)
{
  GrubChooseDefaultButtonBox *bbox = GRUB_CHOOSE_DEFAULT_BUTTON_BOX (user_data);
  GrubChooseDefaultButtonBoxPrivate *priv = GET_PRIVATE (bbox);
  GrubChooseDefaultWidgetInterface * widget_class = g_type_default_interface_peek (GRUB_CHOOSE_DEFAULT_TYPE_WIDGET);

  const gchar *label;

  label = g_object_get_data (G_OBJECT (button), "entry");

  g_assert (label != NULL);

  priv->def_entry = g_strdup (label);

  g_debug ("Pressed %s", label);

  g_signal_emit (bbox, widget_class->signals[GRUB_CHOOSE_DEFAULT_WIDGET_SIGNAL_SELECTED], 0, label);

  if (priv->autocommit)
  {
    commit (GRUB_CHOOSE_DEFAULT_WIDGET (bbox), NULL);
  }
}

static gboolean
commit (GrubChooseDefaultWidget * widget, GError **report_error)
{
  GrubChooseDefaultButtonBox *bbox = GRUB_CHOOSE_DEFAULT_BUTTON_BOX (widget);
  GrubChooseDefaultButtonBoxPrivate *priv = GET_PRIVATE (bbox);

  GError *my_error = NULL;
  GError **error;
  gboolean r;

  if (report_error == NULL)
    error = &my_error;
  else
    error = report_error;

  r = gchd_set_default_entry (priv->gchd, priv->def_entry, error);

  if (report_error == NULL)
  {
    grub_choose_default_error (gtk_widget_get_toplevel (GTK_WIDGET (widget)),
                               my_error);
    g_error_free (my_error);
  }

  return r;
}

/*******************/
/*- public methods-*/
/*******************/

GrubChooseDefaultButtonBox*
grub_choose_default_button_box_new (void)
{
  return g_object_new (GRUB_CHOOSE_DEFAULT_TYPE_BUTTON_BOX, NULL);
}
