/*
  Copyright (c) 2007,2008 TUBITAK/UEKAE

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  Please read the COPYING file.
*/

#ifndef SERVICE_H
#define SERVICE_H

#define POLICYKITKDE_MAX_TRY 3
#define POLICYKITKDE_TIMEOUT 30000

#include <csignal>

#include <polkit/polkit.h>
#include <polkit-grant/polkit-grant.h>

#include <qobject.h>
#include <qmap.h>
#include <qsocketnotifier.h>
#include <qeventloop.h>
#include <qtimer.h>

#include "qdbusconnection.h"
#include "qdbusobject.h"

using namespace std;

class AuthDialog;

class PolicyService: public QObject, public QDBusObjectBase
{
    Q_OBJECT

public:
    PolicyService(QDBusConnection sessionBus);
    virtual ~PolicyService();

    void userSelected(const QString &user);

protected:
    bool handleMethodCall(const QDBusMessage& message);
    void handleIntrospect(const QDBusMessage& message);
    void handleObtainAuthorization(const QDBusMessage& message);
    void sendDBusError(const QDBusMessage& message, const QString& errortype, const QString& errorstr = "org.freedesktop.DBus.Error.InvalidSignature");
    void obtainAuthorization(const QString& actionId, const uint wid, const uint pid, const QDBusMessage& );

protected slots:
    void handleDBusSignals(const QDBusMessage& msg);
    void contextWatchActivated(int fd);
    void grantWatchActivated(int fd);
    void quitSlot(void);

private:
    QDBusConnection m_sessionBus;
    AuthDialog *m_dialog;

    PolKitContext *m_context;
    PolKitGrant *m_grant;
    PolKitError *m_error;

    bool m_authInProgress;
    bool m_gainedPrivilege;
    bool m_inputBogus;
    bool m_cancelled;
    bool m_resetForUser;
    bool m_newUserSelected;

    QString m_userSelected;

    static PolicyService* m_self;
    QString m_uniqueSessionName;
    QTimer *exitTimer;

    QMap<int, QSocketNotifier*> m_contextwatches;
    QMap<int, QSocketNotifier*> m_grantwatches;

    static int polkit_context_add_watch(PolKitContext *context, int fd);
    static void polkit_context_remove_watch(PolKitContext *context, int fd);

    // functions required by polkit_grant_set_functions
    static int polkit_grant_add_watch(PolKitGrant *grant, int fd);
    static int polkit_grant_add_child_watch(PolKitGrant *grant, pid_t pid);
    static void polkit_grant_remove_watch(PolKitGrant *grant, int fd);
    static void polkit_grant_type(PolKitGrant *grant, PolKitResult result, void *data);
    static char *polkit_grant_select_admin_user(PolKitGrant *grant, char **adminUsers, void *data);
    static char *polkit_grant_prompt_echo_off(PolKitGrant *grant, const char *prompt, void *data);
    static char *polkit_grant_prompt_echo_on(PolKitGrant *grant, const char *prompt, void *data);
    char *polkit_grant_prompt(const QString &prompt, bool echo);
    static void polkit_grant_error_message(PolKitGrant *grant, const char *error, void *data);
    static void polkit_grant_text_info(PolKitGrant *grant, const char *info, void *data);
    static PolKitResult polkit_grant_override_grant_type(PolKitGrant *grant, PolKitResult result, void *data);
    static void  polkit_grant_done(PolKitGrant *grant, polkit_bool_t gained_privilege, polkit_bool_t invalid_data, void *data);
    static void polkit_config_changed(PolKitContext *context, void *data);
    static void polkit_grant_sigchld_handler(int sig, siginfo_t *, void *);
};

#endif
