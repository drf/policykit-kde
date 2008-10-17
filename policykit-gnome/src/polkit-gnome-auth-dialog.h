/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-
 *
 * Copyright (C) 2007 David Zeuthen <david@fubar.dk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 */

#ifndef POLKIT_GNOME_AUTH_DIALOG_H
#define POLKIT_GNOME_AUTH_DIALOG_H

#include <gtk/gtkdialog.h>

G_BEGIN_DECLS

#define KEY_AUTH_DIALOG_RETAIN_AUTHORIZATION "/desktop/gnome/policykit/auth_dialog_retain_authorization"
#define KEY_AUTH_DIALOG_RETAIN_AUTHORIZATION_BLACKLIST "/desktop/gnome/policykit/auth_dialog_retain_authorization_blacklist"

#define POLKIT_GNOME_TYPE_AUTH_DIALOG            (polkit_gnome_auth_dialog_get_type ())
#define POLKIT_GNOME_AUTH_DIALOG(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), POLKIT_GNOME_TYPE_AUTH_DIALOG, PolkitGnomeAuthDialog))
#define POLKIT_GNOME_AUTH_DIALOG_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), POLKIT_GNOME_TYPE_AUTH_DIALOG, PolkitGnomeAuthDialogClass))
#define POLKIT_GNOME_IS_AUTH_DIALOG(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), POLKIT_GNOME_TYPE_AUTH_DIALOG))
#define POLKIT_GNOME_IS_AUTH_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), POLKIT_GNOME_TYPE_AUTH_DIALOG))

typedef struct _PolkitGnomeAuthDialog        PolkitGnomeAuthDialog;
typedef struct _PolkitGnomeAuthDialogClass   PolkitGnomeAuthDialogClass;
typedef struct _PolkitGnomeAuthDialogPrivate PolkitGnomeAuthDialogPrivate;

struct _PolkitGnomeAuthDialog
{
	GtkDialog gtk_dialog;
	PolkitGnomeAuthDialogPrivate *priv;
};

struct _PolkitGnomeAuthDialogClass
{
	GtkDialogClass parent_class;

	/* Signals: */

	void (*user_selected) (PolkitGnomeAuthDialog *auth_dialog, const char *user_name);
};

GType    polkit_gnome_auth_dialog_get_type      (void);
GtkWidget *polkit_gnome_auth_dialog_new (const char *path_to_program,
					 const char *action_id,
					 const char *vendor,
					 const char *vendor_url,
					 const char *icon_name,
					 const char *message_markup);

void        polkit_gnome_auth_dialog_set_prompt (PolkitGnomeAuthDialog *auth_dialog,
						 const char *prompt,
						 gboolean show_chars);

const char *polkit_gnome_auth_dialog_get_password (PolkitGnomeAuthDialog *auth_dialog);

void        polkit_gnome_auth_dialog_set_options (PolkitGnomeAuthDialog *auth_dialog, 
						  gboolean session, 
						  gboolean always,
						  gboolean requires_admin,
						  char **admin_users);

gboolean    polkit_gnome_auth_dialog_get_remember_session (PolkitGnomeAuthDialog *auth_dialog);
gboolean    polkit_gnome_auth_dialog_get_remember_always  (PolkitGnomeAuthDialog *auth_dialog);
gboolean    polkit_gnome_auth_dialog_get_apply_all        (PolkitGnomeAuthDialog *auth_dialog);

void        polkit_gnome_auth_dialog_select_admin_user (PolkitGnomeAuthDialog *auth_dialog, const char *admin_user);

void        polkit_gnome_auth_dialog_indicate_auth_error (PolkitGnomeAuthDialog *auth_dialog);

G_END_DECLS

#endif /* POLKIT_GNOME_AUTH_DIALOG_H */
