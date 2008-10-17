/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/***************************************************************************
 *
 * polkit-gnome-action.h : 
 *
 * Copyright (C) 2007 David Zeuthen, <david@fubar.dk>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 *
 **************************************************************************/

#if !defined (POLKIT_GNOME_COMPILATION) && !defined(_POLKIT_GNOME_INSIDE_POLKIT_GNOME_H)
#error "Only <polkit-gnome/polkit-gnome.h> can be included directly, this file may disappear or change contents."
#endif

#ifndef __POLKIT_GNOME_TOGGLE_ACTION_H__
#define __POLKIT_GNOME_TOGGLE_ACTION_H__

#include <gtk/gtk.h>
#include <polkit/polkit.h>
#include <polkit-gnome/polkit-gnome.h>

G_BEGIN_DECLS

#define POLKIT_GNOME_TYPE_TOGGLE_ACTION            (polkit_gnome_toggle_action_get_type ())
#define POLKIT_GNOME_TOGGLE_ACTION(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), POLKIT_GNOME_TYPE_TOGGLE_ACTION, PolKitGnomeToggleAction))
#define POLKIT_GNOME_TOGGLE_ACTION_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), POLKIT_GNOME_TYPE_TOGGLE_ACTION, PolKitGnomeToggleActionClass))
#define POLKIT_GNOME_IS_TOGGLE_ACTION(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), POLKIT_GNOME_TYPE_TOGGLE_ACTION))
#define POLKIT_GNOME_IS_TOGGLE_ACTION_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), POLKIT_GNOME_TYPE_TOGGLE_ACTION))
#define POLKIT_GNOME_TOGGLE_ACTION_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), POLKIT_GNOME_TYPE_TOGGLE_ACTION, PolKitGnomeToggleActionClass))

typedef struct _PolKitGnomeToggleAction        PolKitGnomeToggleAction;
typedef struct _PolKitGnomeToggleActionPrivate PolKitGnomeToggleActionPrivate;
typedef struct _PolKitGnomeToggleActionClass   PolKitGnomeToggleActionClass;

/**
 * PolKitGnomeToggleAction:
 *
 * The PolKitGnomeToggleAction struct contains only private data members and should not be accessed directly.
 */
struct _PolKitGnomeToggleAction
{
        /*< private >*/
        PolKitGnomeAction parent;
        PolKitGnomeToggleActionPrivate *priv;
};

struct _PolKitGnomeToggleActionClass
{
        PolKitGnomeActionClass parent_class;

        void (* toggled) (PolKitGnomeToggleAction *toggle_action);

        /* Padding for future expansion */
        void (*_reserved1) (void);
        void (*_reserved2) (void);
        void (*_reserved3) (void);
        void (*_reserved4) (void);
};

GType                    polkit_gnome_toggle_action_get_type          (void) G_GNUC_CONST;
PolKitGnomeToggleAction *polkit_gnome_toggle_action_new               (const gchar *name);
PolKitGnomeToggleAction *polkit_gnome_toggle_action_new_default (const gchar  *name, 
                                                                 PolKitAction *polkit_action, 
                                                                 const gchar  *locked_label, 
                                                                 const gchar  *unlocked_label);

GtkWidget *polkit_gnome_toggle_action_create_toggle_button (PolKitGnomeToggleAction *action);

G_END_DECLS

#endif  /* __POLKIT_GNOME_TOGGLE_ACTION_H__ */
