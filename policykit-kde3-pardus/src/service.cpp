/*
  Copyright (c) 2007,2008 TUBITAK/UEKAE

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  Please read the COPYING file.
*/

//headers required for SIGCHLD handling and strdup
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <cstring>

#include <sys/types.h>
#include <pwd.h>

//kde and qt headers
#include <qsocketnotifier.h>
#include <qtimer.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <kcombobox.h>
#include <klineedit.h>
#include <kcmdlineargs.h>
#include <kapplication.h>

//policykit headers
#include <polkit/polkit.h>
#include <polkit-dbus/polkit-dbus.h>
#include <polkit-grant/polkit-grant.h>

//dbus backport headers
#include "qdbuserror.h"
#include "qdbusmessage.h"
#include "qdbusdata.h"

//own headers
#include "service.h"
#include "policykitkde.h"
#include "authdialog.h"
#include "debug.h"

using namespace std;

PolicyService* PolicyService::m_self;

PolicyService::PolicyService(QDBusConnection sessionBus): QObject()
{
    //since kde strips 'no', we have to write -exit to control no-exit option
    if (KCmdLineArgs::parsedArgs()->isSet("-exit"))
    {
        //exit, if no-exit option is not set
        Debug::printWarning(QString("no-exit option is not set, setting timer to exit in %1 seconds...").arg(POLICYKITKDE_TIMEOUT / 1000));

        exitTimer = new QTimer();
        exitTimer->connect(exitTimer, SIGNAL(timeout()), this, SLOT(quitSlot(void)));
        exitTimer->start(POLICYKITKDE_TIMEOUT, true);
    }
    else
        Debug::printDebug("no-exit option is set, not quiting");

    Q_ASSERT(!m_self);
    m_self = this;

    m_sessionBus = sessionBus;
    m_error = NULL;
    m_grant = NULL;
    m_dialog = NULL;
    m_authInProgress = false;
    m_gainedPrivilege = false;
    m_inputBogus = false;
    m_cancelled = false;
    m_newUserSelected = false;
    m_resetForUser = false;
    m_userSelected = "";
    m_uniqueSessionName = m_sessionBus.uniqueName();

    Debug::printDebug("Registering object: /");
    if (!m_sessionBus.registerObject("/", this))
    {
        QString msg("Could not register \"/\" object, exiting");
        Debug::printError(msg);
        throw msg;
    }

    //connect all signals to this method
    //m_sessionBus.connect(this, SLOT(handleDBusSignals(const QDBusMessage&)));

    m_context = polkit_context_new();
    if (m_context == NULL)
    {
        QString msg("Could not get a new PolKitContext.");
        Debug::printError(msg);
        throw msg;
    }

    polkit_context_set_load_descriptions(m_context);
    polkit_context_set_config_changed (m_context, polkit_config_changed, NULL);

    polkit_context_set_io_watch_functions (m_context, polkit_context_add_watch, polkit_context_remove_watch);

    if (!polkit_context_init (m_context, &m_error))
    {
        QString msg("Could not initialize PolKitContext");
        if (polkit_error_is_set(m_error))
        {
            Debug::printError(msg + ": " + polkit_error_get_error_message(m_error));
            polkit_error_free(m_error);
        }
        else
            Debug::printError(msg);

        throw msg;
    }
}

void PolicyService::quitSlot()
{
    Debug::printWarning("Timeout limit reached and no-exit option is not set, quiting...");

    //TODO: Do last jobs

    KApplication::kApplication()->quit();
}

PolicyService::~PolicyService()
{
    Debug::printDebug(QString("Unregistering object: %1").arg(POLICYKITKDE_OBJECTNAME));
    m_sessionBus.unregisterObject(POLICYKITKDE_OBJECTNAME);

    if (KCmdLineArgs::parsedArgs()->isSet("-exit"))
        delete exitTimer;
}

bool PolicyService::handleMethodCall(const QDBusMessage& message)
{
    Debug::printDebug(QString("DBus method called: '%1'").arg(message.member()));

    if (message.interface() == "org.freedesktop.DBus.Introspectable" && message.member() == "Introspect")
    {
        if (message.count() != 0)
        {
            sendDBusError(message, "No argument expected");
            return false;
        }
        else
        {
            handleIntrospect(message);
            return true;
        }
    }

    else if (message.interface() == POLICYKITKDE_INTERFACENAME && message.member() == "ObtainAuthorization")
    {
        if (message.count() != 3 || message[0].type() != QDBusData::String || message[1].type() != QDBusData::UInt32 || message[2].type() != QDBusData::UInt32)
        {
            sendDBusError(message, "Wrong signature, three arguments expected: (String, UINT, UINT)");
            return false;
        }
        else
        {
            try
            {
                handleObtainAuthorization(message);
            }
            catch(QString exc)
            {
                return false;
            }
            return true;
        }
    }

    else
    {
        Debug::printWarning(QString("No such DBus method: '%1'").arg(message.member()));
        return false;
    }
}

void PolicyService::handleDBusSignals(const QDBusMessage& msg)
{

    // our service has changed
    if (msg.member() == "NameOwnerChanged" && msg.count() == 3 && msg[1].toString() == m_uniqueSessionName)
    {
        Debug::printWarning(QString("Session bus name owner changed: service name='%1', old owner='%2', new owner='%3'").arg(msg[0].toString()).arg(msg[1].toString()).arg(msg[2].toString()));

        //TODO: exit if not busy
        //polkit_grant_cancel_auth (grant);

    }
}

void PolicyService::sendDBusError(const QDBusMessage& message, const QString& errorstr, const QString& errortype)
{
        Debug::printError(QString("Method call error:'%1'  Interface:'%2' Method:'%3'").arg(errorstr).arg(message.interface()).arg(message.member()));

        QDBusError error(errortype, errorstr);
        QDBusMessage reply = QDBusMessage::methodError(message, error);

        m_sessionBus.send(reply);
}

void PolicyService::handleIntrospect(const QDBusMessage& message)
{
    QDBusMessage reply = QDBusMessage::methodReply(message);

    QString introspection = ""
"<!DOCTYPE node PUBLIC \"-//freedesktop//DTD D-BUS Object Introspection 1.0//EN\"\n"
"\"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd\">\n"
"<node>\n"
"   <interface name=\"org.freedesktop.DBus.Introspectable\">\n"
"       <method name=\"Introspect\">\n"
"           <arg name=\"xml_data\" type=\"s\" direction=\"out\" />\n"
"       </method>\n"
"   </interface>\n"
"   <interface name=\"org.freedesktop.PolicyKit.AuthenticationAgent\">\n"
"       <method name=\"ObtainAuthorization\" >\n"
"           <!-- IN: PolicyKit action identifier; see PolKitAction -->\n"
"           <arg direction=\"in\" type=\"s\" name=\"action_id\" />\n"
"           <!-- IN: X11 window ID for the top-level X11 window the dialog will be transient for. -->\n"
"           <arg direction=\"in\" type=\"u\" name=\"xid\" />\n"
"           <!-- IN: Process ID to grant authorization to -->\n"
"           <arg direction=\"in\" type=\"u\" name=\"pid\" />\n"
"           <!-- OUT: whether the user gained the authorization -->\n"
"           <arg direction=\"out\" type=\"b\" name=\"gained_authorization\" />\n"
"       </method>\n"
"   </interface>\n"
"</node>";

    Debug::printDebug("Handling introspect() call.");
    reply << QDBusData::fromString(introspection);
    m_sessionBus.send(reply);
}

void PolicyService::handleObtainAuthorization(const QDBusMessage& message)
{
    Debug::printDebug("Handling obtainAuthorization() call.");

    obtainAuthorization(message[0].toString(), message[1].toUInt32(), message[2].toUInt32(), message);
}

/////////// PolKit IO watch functions ////////////////
void PolicyService::contextWatchActivated(int fd)
{
    Q_ASSERT(m_contextwatches.contains(fd));

    Debug::printDebug("Context watch activated");

    polkit_context_io_func (m_context, fd);
}

int PolicyService::polkit_context_add_watch(PolKitContext *context, int fd)
{
    Debug::printDebug(QString("polkit_context_add_watch:: Adding watch, fd=%1").arg(fd));

    QSocketNotifier *notify = new QSocketNotifier(fd, QSocketNotifier::Read);
    m_self->m_contextwatches[fd] = notify;

    notify->connect(notify, SIGNAL(activated(int)), m_self, SLOT(contextWatchActivated(int)));

    Debug::printDebug(QString("polkit_context_add_watch:: Watch added, fd=%1").arg(fd));

    // policykit requires a result != 0
    return 1;
}

void PolicyService::polkit_context_remove_watch(PolKitContext *context, int fd)
{
    Debug::printDebug("polkit_context_remove_watch: Removing watch...");
    Q_ASSERT(m_self->m_contextwatches.contains(fd));

    QSocketNotifier* notify = m_self->m_contextwatches[fd];
    delete notify;

    m_self->m_contextwatches.remove(fd);
    Debug::printDebug("polkit_context_remove_watch: Watch removed");
}

void PolicyService::grantWatchActivated(int fd)
{
    Q_ASSERT(m_grantwatches.contains(fd));
    Q_ASSERT(m_grant != NULL);

    //This floods screen
    //Debug::printDebug("Grant watch activated");

    polkit_grant_io_func (m_grant, fd);
}

int PolicyService::polkit_grant_add_watch(PolKitGrant *grant, int fd)
{
    Q_ASSERT(m_self->m_grant != NULL);
    Debug::printDebug(QString("polkit_grant_add_watch: Adding watch, fd=%1").arg(fd));

    QSocketNotifier *notify = new QSocketNotifier(fd, QSocketNotifier::Read);
    m_self->m_grantwatches[fd] = notify;

    notify->connect(notify, SIGNAL(activated(int)), m_self, SLOT(grantWatchActivated(int)));

    Debug::printDebug(QString("polkit_grant_add_watch: Watch added, fd=%1").arg(fd));

    return fd;
}

int PolicyService::polkit_grant_add_child_watch(PolKitGrant *grant, pid_t pid)
{
    Debug::printDebug(QString("polkit_grant_add_child_watch: Adding watch, fd=%1").arg(pid));

    //TODO: Do this in a KDE/Qt way
    struct sigaction *sigac = (struct sigaction *)calloc(1, sizeof(struct sigaction));

    sigac->sa_sigaction = m_self->polkit_grant_sigchld_handler;
    sigac->sa_flags = SA_SIGINFO;

    if (sigaction(SIGCHLD, sigac, NULL) != 0)
        Debug::printError("polkit_grant_add_child_watch: SIGCHLD action could not be changed.");
    else
        Debug::printDebug(QString("polkit_grant_add_child_watch: Watch added for %1").arg(pid));

    free(sigac);
    return pid;
}

void PolicyService::polkit_grant_sigchld_handler(int sig, siginfo_t *info, void *ucontext)
{
    Debug::printDebug(QString("polkit_grant_sigchld_handler: Received SIGCHLD, child exit status=%1 PID=%2").arg(info->si_status).arg(info->si_pid));

    //this should be called when child dies
    polkit_grant_child_func (m_self->m_grant, info->si_pid, info->si_status);
}

void PolicyService::polkit_grant_remove_watch(PolKitGrant *grant, int fd)
{
    Q_ASSERT(m_self->m_grant != NULL);

    Debug::printDebug(QString("polkit_grant_remove_watch: Removing watch, fd=%1").arg(fd));
    Q_ASSERT(m_self->m_grantwatches.contains(fd));

    QSocketNotifier* notify = m_self->m_grantwatches[fd];
    delete notify;

    m_self->m_grantwatches.remove(fd);
    Debug::printDebug(QString("polkit_grant_remove_watch: Watch removed, fd=%1").arg(fd));
}

////////////////////// polkit-grant functions ////////////////////////////

void PolicyService::polkit_grant_type(PolKitGrant *grant, PolKitResult result, void *data)
{
    Q_ASSERT(m_self->m_dialog != NULL);

    Debug::printDebug(QString("polkit_grant_type: Type of authentication dialog is set to \"%1\"").arg(polkit_result_to_string_representation(result)));
    m_self->m_dialog->setType(result);
}

void PolicyService::polkit_config_changed(PolKitContext *context, void *data)
{
    Debug::printWarning("polkit_config_changed: PolicyKit configuration changed");
}

char *PolicyService::polkit_grant_select_admin_user(PolKitGrant *grant, char **adminUsers, void *data)
{
    QStringList list;
    int dialogResult;

    //get username
    passwd *pw = getpwuid(getuid());
    char *username = pw->pw_name;

    Debug::printDebug("polkit_grant_select_admin_user: Setting user list...");

    for (int n = 0; adminUsers[n] != NULL; n++) {
        list.append(adminUsers[n]);
    }

    if (list.contains(username) == 1)
    {
        Debug::printDebug("polkit_grant_select_admin_user: User is in admin group, preselecting...");
        m_self->m_dialog->setAdminUsers(list, username);
    }
    else
    {
        if (list.contains("root") == 1)
            m_self->m_dialog->setAdminUsers(list, "root");
        else
            m_self->m_dialog->setAdminUsers(list);
    }

    Debug::printDebug("polkit_grant_select_admin_user: Done");

    char *selected;

    if (m_self->m_newUserSelected)
    {
        Debug::printDebug(QString("polkit_grant_select_admin_user: New user(%1) selected").arg(m_self->m_userSelected));
        selected = strdup(m_self->m_userSelected);
    }
    else
    {
        Debug::printDebug(QString("polkit_grant_select_admin_user: First time of select_admin_user, user: %1").arg(m_self->m_dialog->cbUsers->currentText()));
        selected = strdup(m_self->m_dialog->cbUsers->currentText());
    }

    return selected;

}

char *PolicyService::polkit_grant_prompt(const QString &prompt, bool echo)
{
    //TODO: check prompt like polkit-gnome

    m_dialog->setPrompt(prompt, m_self->m_userSelected);

    if (echo)
        m_dialog->lePassword->setEchoMode(QLineEdit::Normal);
    else
        m_dialog->lePassword->setEchoMode(QLineEdit::Password);

    int result = m_dialog->exec();

    if (result == QDialog::Rejected)
    {
        Debug::printDebug("polkit_grant_prompt: Dialog cancelled");
        m_cancelled = true;
        polkit_grant_cancel_auth (m_grant);
        return NULL;
    }

    //In order to be freed by PolKitGrant class without crash, this is needed
    char *answer = strdup(m_dialog->getPass());
    return answer;
}

char *PolicyService::polkit_grant_prompt_echo_off(PolKitGrant *grant, const char *prompt, void *data)
{
    Debug::printDebug(QString("polkit_grant_prompt_echo_off: prompt=\"%1\"").arg(prompt));
    return m_self->polkit_grant_prompt(prompt, false);
}

char *PolicyService::polkit_grant_prompt_echo_on(PolKitGrant *grant, const char *prompt, void *data)
{
    Debug::printDebug(QString("polkit_grant_prompt_echo_on: prompt=\"%1\"").arg(prompt));
    return m_self->polkit_grant_prompt(prompt, true);
}

void PolicyService::polkit_grant_error_message(PolKitGrant *grant, const char *error, void *data)
{
    Debug::printError(QString("polkit_grant_error_message: %1").arg(error));
}

void PolicyService::polkit_grant_text_info(PolKitGrant *grant, const char *info, void *data)
{
    Debug::printDebug(QString("polkit_grant_text_info: %1").arg(info));
}

PolKitResult PolicyService::polkit_grant_override_grant_type(PolKitGrant *grant, PolKitResult result, void *data)
{
    Debug::printDebug("In polkit_grant_override_grant_type...");

    PolKitResult overridden = result;
    bool remember = m_self->m_dialog->bgRemember->isChecked();
    bool rememberCheck = m_self->m_dialog->cbRemember->isChecked();
    bool keepSession = m_self->m_dialog->rbSession->isOn();
    bool keepAlways = m_self->m_dialog->rbAlways->isOn();

    Debug::printDebug(QString("polkit_grant_override_grant_type: keep session: %1, keep always: %2").arg(keepSession).arg(keepAlways));

    switch(result)
    {
        // result can not be overridden if keepsession or keepalways do not exist
        case POLKIT_RESULT_ONLY_VIA_ADMIN_AUTH_ONE_SHOT:
        case POLKIT_RESULT_ONLY_VIA_SELF_AUTH_ONE_SHOT:
        case POLKIT_RESULT_ONLY_VIA_ADMIN_AUTH:
        case POLKIT_RESULT_ONLY_VIA_SELF_AUTH:
            overridden = result;
            break;
        case POLKIT_RESULT_ONLY_VIA_ADMIN_AUTH_KEEP_SESSION:
            //if keepsession is available but user does not select it, override result with adminauth
            if (!rememberCheck)
                overridden = POLKIT_RESULT_ONLY_VIA_ADMIN_AUTH;
            else
                overridden = result;
            break;
        case POLKIT_RESULT_ONLY_VIA_ADMIN_AUTH_KEEP_ALWAYS:
            //if keepalways and keepsession options are available but user does not select them, override result with adminauth
            if (!keepAlways && !keepSession)
                overridden = POLKIT_RESULT_ONLY_VIA_ADMIN_AUTH;
            else if (keepSession)
                overridden = POLKIT_RESULT_ONLY_VIA_ADMIN_AUTH_KEEP_SESSION;
            else
                overridden = POLKIT_RESULT_ONLY_VIA_ADMIN_AUTH_KEEP_ALWAYS;
            break;

        case POLKIT_RESULT_ONLY_VIA_SELF_AUTH_KEEP_SESSION:
            if (!rememberCheck)
                overridden = POLKIT_RESULT_ONLY_VIA_SELF_AUTH;
            break;
        case POLKIT_RESULT_ONLY_VIA_SELF_AUTH_KEEP_ALWAYS:
            if (!keepAlways && !keepSession)
                overridden = POLKIT_RESULT_ONLY_VIA_SELF_AUTH;
            else if (keepSession)
                overridden = POLKIT_RESULT_ONLY_VIA_SELF_AUTH_KEEP_SESSION;
            else
                overridden = POLKIT_RESULT_ONLY_VIA_SELF_AUTH_KEEP_ALWAYS;
            break;
        default:
            Debug::printWarning("polkit_grant_override_grant_type: Unexpected PolKitResult type");
    }

    Debug::printDebug(QString("polkit_grant_override_grant_type: default type = %1, overridden type = %2").arg(polkit_result_to_string_representation(result)).arg(polkit_result_to_string_representation(overridden)));
    return overridden;
}

void PolicyService::polkit_grant_done(PolKitGrant *grant, polkit_bool_t gained_privilege, polkit_bool_t invalid_data, void *data)
{
    Debug::printDebug(QString("In polkit_grant_done: gained_privilege=%1 invalid_data=%2").arg(gained_privilege).arg(invalid_data));
    m_self->m_gainedPrivilege = gained_privilege;
    m_self->m_inputBogus= invalid_data;
    QApplication::eventLoop()->exit();
}

void PolicyService::obtainAuthorization(const QString& actionId, const uint wid, const uint pid, const QDBusMessage& messageToReply)
{
    if (m_authInProgress)
    {
        Debug::printError("obtainAuthorization: Another agent is already authenticating.");

        QDBusMessage replyError = QDBusMessage::methodReply(messageToReply);

        replyError << QDBusData::fromBool(false);
        m_sessionBus.send(replyError);

        return;
    }

    m_authInProgress = true;

    //stop exitTimer during authentication, and restart when it is finished
    if (KCmdLineArgs::parsedArgs()->isSet("-exit"))
    {
        Debug::printDebug("Authentication is in progress, stopping timer");
        exitTimer->stop();
    }

    PolKitAction *action = polkit_action_new();
    if (action == NULL)
    {
        QString msg = QString("Could not create new action");
        Debug::printError(msg);
        m_authInProgress = false;
        throw msg;
    }

    polkit_bool_t setActionResult = polkit_action_set_action_id(action, actionId.ascii());
    if (!setActionResult)
    {
        QString msg = QString("Could not set actionid.");
        Debug::printError(msg);
        m_authInProgress = false;
        throw msg;
    }

    Debug::printDebug("Getting policy cache...");
    PolKitPolicyCache *cache = polkit_context_get_policy_cache(m_context);
    if (cache == NULL)
        Debug::printWarning("Could not get policy cache.");

    Debug::printDebug("Getting policy cache entry for an action...");
    PolKitPolicyFileEntry *entry = polkit_policy_cache_get_entry(cache, action);
    if (entry == NULL)
        Debug::printWarning("Could not get policy entry for action.");

    Debug::printDebug("Getting action message...");
    const char *message = polkit_policy_file_entry_get_action_message(entry);

    if (message == NULL)
        Debug::printWarning("Could not get action message for action.");
    else
        Debug::printDebug(QString("Message of action: '%1'").arg(message));

    DBusError dbuserror;
    dbus_error_init (&dbuserror);
    DBusConnection *systemBus = dbus_bus_get (DBUS_BUS_SYSTEM, &dbuserror);
    if (systemBus == NULL) 
    {
        QString msg = QString("Could not connect to system bus.");
        Debug::printError(msg);
        m_authInProgress = false;
        throw msg;
    }

    PolKitCaller *caller = polkit_caller_new_from_pid(systemBus, pid, &dbuserror);
    if (caller == NULL)
    {
        QDBusError *qerror = new QDBusError((const DBusError *)&dbuserror);
        QString msg = QString("Could not define caller from pid: %1").arg(qerror->message());
        Debug::printError(msg);
        m_authInProgress = false;
        throw msg;
    }

    //TODO: Add vendor icon support

    for (int i = 0; i < POLICYKITKDE_MAX_TRY; i++)
    {
        m_grant = polkit_grant_new();

        if (m_grant == NULL)
        {
            QString msg = QString("PolKitGrant object could not be created");
            Debug::printError(msg);
            m_authInProgress = false;
            throw msg;
        }

        QString qMessage = QString::fromUtf8(message);
        m_dialog = new AuthDialog(qMessage, this);
        Debug::printDebug("AuthDialog created.");

        polkit_grant_set_functions(m_grant,
                                   polkit_grant_add_watch,
                                   polkit_grant_add_child_watch,
                                   polkit_grant_remove_watch,
                                   polkit_grant_type,
                                   polkit_grant_select_admin_user,
                                   polkit_grant_prompt_echo_off,
                                   polkit_grant_prompt_echo_on,
                                   polkit_grant_error_message,
                                   polkit_grant_text_info,
                                   polkit_grant_override_grant_type,
                                   polkit_grant_done,
                                   NULL);

        // explicitly set to false before every try
        m_gainedPrivilege = false;
        m_inputBogus = false;
        m_cancelled = false;

        if (!polkit_grant_initiate_auth (m_grant, action, caller)) 
        {
            QString msg = QString("Could not initialize grant");
            Debug::printError(msg);
            m_authInProgress = false;
            delete m_dialog;
            if (m_grant)
                polkit_grant_unref(m_grant);
            throw msg;
        }

        // This workaround used for to avoid ourself from a race condition,
        // polkit_grant_done must return before the following privilege check
        Debug::printDebug("obtain_authorization: Entering eventloop to wait grant_done");
        QApplication::eventLoop()->exec();

        if(m_newUserSelected && m_resetForUser)
        {
            Debug::printDebug("obtain_authorization: New user selected, restarting authentication process...");

            if (m_dialog)
                delete m_dialog;

            if (m_grant)
                polkit_grant_unref (m_grant);

            i--;
            m_resetForUser = false;
            continue;
        }

        if (!m_gainedPrivilege && !m_inputBogus && !m_cancelled)
        {
            Debug::printDebug("obtain_authorization: Authentication failure, trying again...");
            if (m_grant)
                polkit_grant_unref (m_grant);
        }
        else if (!m_gainedPrivilege && m_inputBogus && !m_cancelled)
        {
            Debug::printWarning("obtain_authorization: Authentication failure, you may already have authentication for this action");
            break;
        }
        else
        {
            m_userSelected = "";
            m_newUserSelected = false;
            m_resetForUser = false;
            break;
        }

    }

    if (m_grant)
        polkit_grant_unref (m_grant);
    if (action)
        polkit_action_unref (action);
    if (caller)
        polkit_caller_unref (caller);

    Debug::printDebug(QString("obtain_authorization: privilege: %1 input_bogus: %2, cancelled: %3").arg(m_gainedPrivilege).arg(m_inputBogus).arg(m_cancelled));

    //send dbus reply
    QDBusMessage reply = QDBusMessage::methodReply(messageToReply);

    reply << QDBusData::fromBool(m_gainedPrivilege);
    m_sessionBus.send(reply);

    if (KCmdLineArgs::parsedArgs()->isSet("-exit"))
    {
        Debug::printDebug("Authentication finished, starting timer again");
        exitTimer->start(POLICYKITKDE_TIMEOUT, true);
    }

    m_authInProgress = false;
    delete m_dialog;
}

void PolicyService::userSelected(const QString &user)
{
    Debug::printDebug(QString("User selected: %1").arg(user));
    m_newUserSelected = true;
    m_resetForUser = true;
    m_userSelected = user;

    polkit_grant_cancel_auth (m_grant);

    //close dialog
    m_dialog->close();

}

#include "service.moc"
