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
#include "grub-choose-default-widget.h"
#include "grub-choose-default-button-box.h"

/*- private prototypes -*/

static void update_reboot (GrubChooseDefaultWindow *win);
static void perform_reboot (GrubChooseDefaultWindow *win);
static gboolean tryandrun (GrubChooseDefaultWindow *win, const gchar * directory);
static void handle_selected (GrubChooseDefaultWidget *box, const gchar * entry, gpointer data);
static void handle_cancel (GtkButton *button, gpointer data);
static void handle_reboot (GtkToggleButton *button, gpointer data);


/*- globals -*/

enum {
  PROP_0,
  PROP_REBOOT,
};


/*****************/
/*- class setup -*/
/*****************/

G_DEFINE_TYPE (GrubChooseDefaultWindow, grub_choose_default_window, GTK_TYPE_DIALOG)

#define GET_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), GRUB_CHOOSE_DEFAULT_TYPE_WINDOW, GrubChooseDefaultWindowPrivate))

typedef struct _GrubChooseDefaultWindowPrivate GrubChooseDefaultWindowPrivate;

struct _GrubChooseDefaultWindowPrivate {
  GrubChooseDefaultWidget * box;
  gboolean reboot;
  GtkWidget *check_reboot;
};

static void
grub_choose_default_window_get_property (GObject *object, guint property_id,
                              GValue *value, GParamSpec *pspec)
{
  GrubChooseDefaultWindowPrivate *priv = GET_PRIVATE (GRUB_CHOOSE_DEFAULT_WINDOW (object));

  switch (property_id) {
    case PROP_REBOOT:
      g_value_set_boolean (value, priv->reboot);
      break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
  }
}

static void
grub_choose_default_window_set_property (GObject *object, guint property_id,
                              const GValue *value, GParamSpec *pspec)
{
  GrubChooseDefaultWindowPrivate *priv = GET_PRIVATE (GRUB_CHOOSE_DEFAULT_WINDOW (object));

  switch (property_id) {
    case PROP_REBOOT:
      priv->reboot = g_value_get_boolean (value);
      update_reboot (GRUB_CHOOSE_DEFAULT_WINDOW (object));
      break;
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

  g_object_class_install_property (object_class, PROP_REBOOT,
           g_param_spec_boolean ("reboot", "reboot", "reboot",
                                 FALSE, G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
}

static void
grub_choose_default_window_init (GrubChooseDefaultWindow *self)
{
  GrubChooseDefaultWindowPrivate *priv = GET_PRIVATE (GRUB_CHOOSE_DEFAULT_WINDOW (self));
  GtkWidget *area, *scrolled;
  GtkRequisition req, req_btn;
  GtkWidget *check_reboot;
  GtkWidget *button_cancel;

  area = gtk_dialog_get_content_area (GTK_DIALOG (self));

  scrolled = gtk_scrolled_window_new (NULL, NULL);
  gtk_container_add (GTK_CONTAINER (area), scrolled);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_widget_show (scrolled);

  priv->box = GRUB_CHOOSE_DEFAULT_WIDGET (grub_choose_default_button_box_new ());
  g_signal_connect (priv->box, "selected", G_CALLBACK (handle_selected), self);
  g_object_set (priv->box, "auto-commit", TRUE, NULL);

  gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scrolled), GTK_WIDGET (priv->box));
  gtk_widget_show (GTK_WIDGET (priv->box));


  area = gtk_dialog_get_action_area (GTK_DIALOG (self));
  g_debug ("action area class: %s", G_OBJECT_CLASS_NAME (G_OBJECT_GET_CLASS (area)));

  priv->check_reboot = check_reboot = gtk_check_button_new_with_label ("Logout / Reboot immediately");
  update_reboot (self);
  g_signal_connect (check_reboot, "toggled", G_CALLBACK (handle_reboot), self);

  gtk_container_add (GTK_CONTAINER (area), check_reboot);
  gtk_widget_show (check_reboot);

  button_cancel = gtk_button_new_from_stock (GTK_STOCK_CANCEL);
  gtk_container_add (GTK_CONTAINER (area), button_cancel);
  g_signal_connect (button_cancel, "clicked", G_CALLBACK (handle_cancel), self);
  gtk_widget_show (button_cancel);


  gtk_widget_size_request (GTK_WIDGET (priv->box), &req);
  gtk_widget_size_request (GTK_WIDGET (button_cancel), &req_btn);

  req.height += req_btn.height + 25;
  req.width += 25;

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

static void
update_reboot (GrubChooseDefaultWindow *win)
{
  GrubChooseDefaultWindowPrivate *priv = GET_PRIVATE (win);

  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (priv->check_reboot), priv->reboot);
}

static gboolean
tryandrun (GrubChooseDefaultWindow *win, const gchar * directory)
{
  gchar * script;

  script = g_build_filename (directory, "grub-choose-default", "reboot", NULL);

  g_debug ("Looking for reboot script %s\n", script);

  if (g_file_test (script, G_FILE_TEST_IS_EXECUTABLE))
  {
    gchar *argv[] = { script, NULL };
    GError * error = NULL;
    gboolean r;

    r = g_spawn_async (NULL,
                       argv,
                       NULL,
                       0,
                       NULL,
                       NULL,
                       NULL,
                       &error);

    if (!r)
    {
      g_warning ("%s", error->message);
      g_error_free (error);
    }

    return r;
  }

  return FALSE;
}

static void
perform_reboot (GrubChooseDefaultWindow *win)
{
  //GrubChooseDefaultWindowPrivate *priv = GET_PRIVATE (win);

  gboolean r;
  const gchar * const * config_dirs;
  //const gchar * config_dir;

  r = tryandrun (win, g_get_user_config_dir ());

  if (r)
    return;

  for (config_dirs = g_get_system_config_dirs (); *config_dirs != NULL; config_dirs++)
  {
    r = tryandrun (win, *config_dirs);
    if (r)
      break;
  }
}

static void
handle_selected (GrubChooseDefaultWidget *box, const gchar * entry, gpointer data)
{
  GrubChooseDefaultWindow *win = GRUB_CHOOSE_DEFAULT_WINDOW (data);
  GrubChooseDefaultWindowPrivate *priv = GET_PRIVATE (win);

  if (priv->reboot)
  {
    perform_reboot (win);
  }

  gtk_dialog_response (GTK_DIALOG (win), GTK_RESPONSE_ACCEPT);
}

static void
handle_cancel (GtkButton *button, gpointer data)
{
  GrubChooseDefaultWindow *win = GRUB_CHOOSE_DEFAULT_WINDOW (data);

  gtk_dialog_response (GTK_DIALOG (win), GTK_RESPONSE_CANCEL);
}

static void
handle_reboot (GtkToggleButton *button, gpointer data)
{
  GrubChooseDefaultWindow *win = GRUB_CHOOSE_DEFAULT_WINDOW (data);
  GrubChooseDefaultWindowPrivate *priv = GET_PRIVATE (win);

  priv->reboot = !priv->reboot;
}

/*******************/
/*- public methods-*/
/*******************/

GrubChooseDefaultWindow*
grub_choose_default_window_new (void)
{
  return g_object_new (GRUB_CHOOSE_DEFAULT_TYPE_WINDOW, NULL);
}
