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

#include <config.h>

#if HAVE_STRING_H
#  include <string.h>
#endif

#include <glib/gstdio.h>

#include "grub-choose-default-window.h"
#include "grub-choose-default-widget.h"
#include "grub-choose-default-button-box.h"
#include "grub-choose-default-util.h"
#include "gchd-error.h"
#include "gchd-util.h"
#include "gchd.h"

#define MAIN_GROUP "Settings"

/*- private prototypes -*/

static void update_reboot (GrubChooseDefaultWindow *win);
static void update_once (GrubChooseDefaultWindow *win);
static void perform_reboot (GrubChooseDefaultWindow *win);
static void handle_selected (GrubChooseDefaultWidget *box, const gchar * entry, gpointer data);
static void handle_cancel (GtkButton *button, gpointer data);
static void handle_reboot (GtkToggleButton *button, gpointer data);
static void handle_once (GtkToggleButton *button, gpointer data);
static void load_settings (GrubChooseDefaultWindow *win);
static void save_settings (GrubChooseDefaultWindow *win);


/*- globals -*/

enum {
  PROP_0,
  PROP_REBOOT,
  PROP_ONCE,
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
  gboolean reboot, once;
  GtkWidget * check_reboot;
  GtkWidget * radio_default;
  GtkWidget * radio_once;
  GKeyFile * config;
  gchar * config_fn;
  gchar * grub_dir;
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
    case PROP_ONCE:
      g_value_set_boolean (value, priv->once);
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
      DBG ("Setting reboot to %d", priv->reboot);
      update_reboot (GRUB_CHOOSE_DEFAULT_WINDOW (object));
      break;
    case PROP_ONCE:
      priv->once = g_value_get_boolean (value);
      DBG ("Setting once to %d", priv->once);
      g_object_set (priv->box,
                    "once", priv->once,
                    NULL);
      update_once (GRUB_CHOOSE_DEFAULT_WINDOW (object));
      break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
  }
}

static void
grub_choose_default_window_finalize (GObject *object)
{
  GrubChooseDefaultWindow *win = GRUB_CHOOSE_DEFAULT_WINDOW (object);
  GrubChooseDefaultWindowPrivate *priv = GET_PRIVATE (win);

  g_key_file_free (priv->config);
  g_free (priv->config_fn);

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
                                 FALSE, G_PARAM_READWRITE));
  g_object_class_install_property (object_class, PROP_ONCE,
           g_param_spec_boolean ("once", "once", "once",
                                 FALSE, G_PARAM_READWRITE));
}

static void
grub_choose_default_window_init (GrubChooseDefaultWindow *self)
{
  GrubChooseDefaultWindowPrivate *priv = GET_PRIVATE (GRUB_CHOOSE_DEFAULT_WINDOW (self));
  GtkWidget *area, *scrolled;
  GtkRequisition req, req_vbox;
  GtkWidget *check_reboot;
  GtkWidget *button_cancel;
  GtkWidget *vbox, *hbox;
  GtkWidget *radio_once, *radio_default;

  priv->config_fn = g_build_filename (g_get_user_config_dir (), "grub-choose-default", "config", NULL);
  priv->config = g_key_file_new ();

  gtk_window_set_icon_name (GTK_WINDOW (self), "grub-choose-default");

  area = gtk_dialog_get_content_area (GTK_DIALOG (self));

  scrolled = gtk_scrolled_window_new (NULL, NULL);
  gtk_container_add (GTK_CONTAINER (area), scrolled);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_widget_show (scrolled);

  priv->box = GRUB_CHOOSE_DEFAULT_WIDGET (grub_choose_default_button_box_new (priv->grub_dir));
  g_signal_connect (priv->box, "selected", G_CALLBACK (handle_selected), self);
  g_object_set (priv->box, "auto-commit", TRUE, NULL);

  gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scrolled), GTK_WIDGET (priv->box));
  gtk_widget_show (GTK_WIDGET (priv->box));


  area = gtk_dialog_get_action_area (GTK_DIALOG (self));

  vbox = gtk_vbox_new (TRUE, 2);
  hbox = gtk_hbox_new (TRUE, 2);

  priv->radio_default = radio_default = gtk_radio_button_new_with_label (NULL, "Set default");
  priv->radio_once = radio_once = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (radio_default),
                                                                               "Next reboot only");
  gtk_box_pack_start (GTK_BOX (hbox), radio_default, TRUE, TRUE, 2);
  gtk_box_pack_start (GTK_BOX (hbox), radio_once, TRUE, TRUE, 2);

  gtk_container_add (GTK_CONTAINER (vbox), hbox);

  priv->check_reboot = check_reboot = gtk_check_button_new_with_label ("End session immediately");

  gtk_container_add (GTK_CONTAINER (vbox), check_reboot);

  gtk_container_add (GTK_CONTAINER (area), vbox);
  gtk_widget_show_all (vbox);

  button_cancel = gtk_button_new_from_stock (GTK_STOCK_CANCEL);
  gtk_container_add (GTK_CONTAINER (area), button_cancel);
  g_signal_connect (button_cancel, "clicked", G_CALLBACK (handle_cancel), self);
  gtk_widget_show (button_cancel);


  gtk_widget_size_request (GTK_WIDGET (priv->box), &req);
  gtk_widget_size_request (GTK_WIDGET (vbox), &req_vbox);

  req.height += req_vbox.height + 25;
  req.width += 25;

  if (req.width > 600 )
    req.width = 600;
  if (req.height > 800)
    req.height = 800;

  DBG ("Will request size %d by %d", req.width, req.height);

  gtk_window_set_default_size (GTK_WINDOW (self), req.width, req.height);

  load_settings (self);
  g_signal_connect (radio_default, "toggled", G_CALLBACK (handle_once), self);
  g_signal_connect (radio_once, "toggled", G_CALLBACK (handle_once), self);
  g_signal_connect (check_reboot, "toggled", G_CALLBACK (handle_reboot), self);
}


/***************/
/*- internals -*/
/***************/

static void
update_reboot (GrubChooseDefaultWindow *win)
{
  GrubChooseDefaultWindowPrivate *priv = GET_PRIVATE (win);

  if (priv->check_reboot)
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (priv->check_reboot), priv->reboot);
}

static void
update_once (GrubChooseDefaultWindow *win)
{
  GrubChooseDefaultWindowPrivate *priv = GET_PRIVATE (win);

  if (priv->once)
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (priv->radio_once), TRUE);
  else
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (priv->radio_default), TRUE);
}

static void
perform_reboot (GrubChooseDefaultWindow *win)
{
  //GrubChooseDefaultWindowPrivate *priv = GET_PRIVATE (win);

  gboolean r;
  const gchar * const * config_dirs;
  const gchar * config_dir;
  gchar * script;
  GError *error = NULL;

  script = g_build_filename (CONFIG_DIR, "reboot", NULL);

  config_dir = g_get_user_config_dir ();

  r = grub_choose_default_exec (config_dir, script, NULL);

  if (r)
  {
    return;
  }

  for (config_dirs = g_get_system_config_dirs (); *config_dirs != NULL; config_dirs++)
  {
    r = grub_choose_default_exec (*config_dirs, script, NULL);
    if (r)
      break;
  }

  /* we found no script */

  g_set_error (&error,
               GCHD_ERROR, GCHD_ERROR_NO_REBOOT_SCRIPT,
               "Could not find any reboot scripts.\nYou can install one in %s/%s .",
               config_dir, script);
  grub_choose_default_error (GTK_WIDGET (win), error);
  g_error_free (error);

  g_free (script);
}

static void
handle_selected (GrubChooseDefaultWidget *box, const gchar * entry, gpointer data)
{
  GrubChooseDefaultWindow *win = GRUB_CHOOSE_DEFAULT_WINDOW (data);
  GrubChooseDefaultWindowPrivate *priv = GET_PRIVATE (win);

  save_settings (win);

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

  save_settings (win);

  gtk_dialog_response (GTK_DIALOG (win), GTK_RESPONSE_CANCEL);
}

static void
handle_reboot (GtkToggleButton *button, gpointer data)
{
  GrubChooseDefaultWindow *win = GRUB_CHOOSE_DEFAULT_WINDOW (data);
  GrubChooseDefaultWindowPrivate *priv = GET_PRIVATE (win);

  priv->reboot = gtk_toggle_button_get_active (button);
}

static void
handle_once (GtkToggleButton *button, gpointer data)
{
  GrubChooseDefaultWindow *win = GRUB_CHOOSE_DEFAULT_WINDOW (data);
  GrubChooseDefaultWindowPrivate *priv = GET_PRIVATE (win);

  /*
   * The signal is created for both buttons, so we can just
   * check if radio_once is enabled or disabled
   */
  if (button == GTK_TOGGLE_BUTTON (priv->radio_once)) {
    g_object_set (win,
                  "once", gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (priv->radio_once)),
                  NULL);
  }
}

static void
load_settings (GrubChooseDefaultWindow *win)
{
  GrubChooseDefaultWindowPrivate *priv = GET_PRIVATE (win);
  GError * error = NULL;
  gboolean r;
  gboolean reboot, once;


  if (!g_file_test (priv->config_fn, G_FILE_TEST_EXISTS))
    return;

  DBG ("Loading settings...");

  r = g_key_file_load_from_file (priv->config, priv->config_fn, G_KEY_FILE_KEEP_COMMENTS, &error);

  if (!r)
  {
    grub_choose_default_error (GTK_WIDGET (win), error);
    return;
  }

  r = g_key_file_has_key (priv->config, MAIN_GROUP, "reboot", &error);

  if (error)
  {
    grub_choose_default_error (GTK_WIDGET (win), error);
    g_error_free (error);
  }

  if (r)
  {
    reboot = g_key_file_get_boolean (priv->config, MAIN_GROUP, "reboot", &error);

    if (error)
    {
      grub_choose_default_error (GTK_WIDGET (win), error);
      g_error_free (error);
    }
    else
    {
      //priv->reboot = reboot;
      g_object_set (win, "reboot", reboot, NULL);
    }
  }


  r = g_key_file_has_key (priv->config, MAIN_GROUP, "once", &error);

  if (error)
  {
    grub_choose_default_error (GTK_WIDGET (win), error);
    g_error_free (error);
  }

  if (r)
  {
    once = g_key_file_get_boolean (priv->config, MAIN_GROUP, "once", &error);

    if (error)
    {
        grub_choose_default_error (GTK_WIDGET (win), error);
        g_error_free (error);
    }
    else
    {
        g_object_set (win, "once", once, NULL);
    }
  }


  if (g_key_file_has_key (priv->config, MAIN_GROUP, "grub_dir", &error))
    priv->grub_dir = g_key_file_get_string (priv->config, MAIN_GROUP, "grub_dir", &error);

  if (error)
  {
    grub_choose_default_error (GTK_WIDGET (win), error);
    g_error_free (error);
  }
}

static void
save_settings (GrubChooseDefaultWindow *win)
{
  GrubChooseDefaultWindowPrivate *priv = GET_PRIVATE (win);
  GError * error = NULL;
  gboolean r;
  gchar * data;
  gchar * dir;
  gsize len;

  DBG ("Saving settings...");

  DBG ("Saving reboot as %d", priv->reboot);
  DBG ("Saving once as %d", priv->once);
  g_key_file_set_boolean (priv->config, MAIN_GROUP, "reboot", priv->reboot);
  g_key_file_set_boolean (priv->config, MAIN_GROUP, "once", priv->once);

  /*
   * Don't save this, it's a user setting 
  g_object_get (priv->box, "gchd", &gchd, NULL);
  g_key_file_set_string (priv->config, MAIN_GROUP, "grub_dir", gchd_get_grub_dir (gchd));
  */
  
  data = g_key_file_to_data (priv->config, &len, &error);

  if (!data)
  {
    grub_choose_default_error (GTK_WIDGET (win), error);
    g_error_free (error);
    return;
  }

  dir = g_path_get_dirname (priv->config_fn);
  if (!g_file_test (priv->config_fn, G_FILE_TEST_EXISTS))
    g_mkdir (dir, 0770);
  g_free (dir);

  r = g_file_set_contents (priv->config_fn, data, len, &error);

  if (!r)
  {
    grub_choose_default_error (GTK_WIDGET (win), error);
    g_error_free (error);
    return;
  }
}


/*******************/
/*- public methods-*/
/*******************/

GrubChooseDefaultWindow*
grub_choose_default_window_new (void)
{
  return g_object_new (GRUB_CHOOSE_DEFAULT_TYPE_WINDOW, NULL);
}
