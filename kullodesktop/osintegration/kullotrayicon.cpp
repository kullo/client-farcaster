/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
#include "kullotrayicon.h"

#include <limits>
#include <QAction>
#include <QMenu>
#include <QSystemTrayIcon>

#include <kulloclient/util/assert.h>
#include <kulloclient/util/librarylogger.h>
#include <kulloclient/util/misc.h>

#include <desktoputil/osdetection.h>

#include "kullodesktop/applications/kulloapplication.h"
#include "kullodesktop/qml/innerapplication.h"

namespace KulloDesktop {
namespace OsIntegration {

namespace {
const auto ICON_STATE_LOGGEDOUT = IconState(false, 0);
const auto ICON_STATE_INVALID = IconState(false, std::numeric_limits<quint32>::max());
}

KulloTrayIcon::KulloTrayIcon(Qml::InnerApplication &app,
                             QObject *parent)
    : QObject(parent)
    , iconState_(ICON_STATE_INVALID)
    , app_(app)
    , trayIcon_(Kullo::make_unique<QSystemTrayIcon>(nullptr))
{
    setIcon(ICON_STATE_LOGGEDOUT);
    trayIcon_->setToolTip(app_.mainApplication().applicationName());

    if (QSystemTrayIcon::supportsMessages())
    {
        Log.i() << "Tray icon messages supported.";
    }
    else
    {
        Log.i() << "Tray icon messages not supported.";
    }

    if (DesktopUtil::OsDetection::windows() || DesktopUtil::OsDetection::linux())
    {
        // Restore on icon click
        connect(trayIcon_.get(), &QSystemTrayIcon::activated,
                this, [this](QSystemTrayIcon::ActivationReason reason)
        {
            if (reason == QSystemTrayIcon::Trigger)
            {
                emit showMainWindowRequested();
            }
        });
    }

    if (!DesktopUtil::OsDetection::windows())
    {
        // Restore on context menu click
        trayIconMenuActions_.emplace_back(Kullo::make_unique<QAction>(trUtf8("Show"), nullptr));
        connect(trayIconMenuActions_.back().get(), &QAction::triggered,
                this, &KulloTrayIcon::showMainWindowRequested);
    }

    trayIconMenuActions_.emplace_back(Kullo::make_unique<QAction>(trUtf8("Quit"), nullptr));
    connect(trayIconMenuActions_.back().get(), &QAction::triggered,
            this, [this]()
    {
        Log.i() << "Quit now!";
        app_.mainApplication().quit();
    });

    trayIconMenu_ = Kullo::make_unique<QMenu>();
    for (auto &action : trayIconMenuActions_)
    {
        trayIconMenu_->addAction(action.get());
    }
    trayIcon_->setContextMenu(trayIconMenu_.get());

    connect(trayIcon_.get(), &QSystemTrayIcon::messageClicked,
            this, &KulloTrayIcon::showMainWindowRequested);

    trayIcon_->show();
}

KulloTrayIcon::~KulloTrayIcon()
{
}

void KulloTrayIcon::onHasSessionChanged(bool hasSession)
{
    if (hasSession)
    {
        setIcon(IconState(true, iconState_.unreadMessages));
    }
    else
    {
        setIcon(ICON_STATE_LOGGEDOUT);
    }
}

void KulloTrayIcon::onUnreadMessagesCountChanged(int count)
{
    Log.d() << "Unread message count changed: " << count;
    setIcon(IconState(true, count));
}

void KulloTrayIcon::onSyncFinished(bool success, int countMessagesNew, int countMessagesNewUnread, int countMessagesModified, int countMessagesDeleted)
{
    K_UNUSED(countMessagesNew);
    K_UNUSED(countMessagesDeleted);
    K_UNUSED(countMessagesModified);

    if (success && countMessagesNewUnread)
    {
        Log.i() << "Incoming unread messages: " << countMessagesNewUnread;

        const auto popupTitle = trUtf8("Kullo message");
        if (countMessagesNewUnread == 1)
        {
            trayIcon_->showMessage(popupTitle,
                                   trUtf8("One new message received"));
        }
        else
        {
            trayIcon_->showMessage(popupTitle,
                                   trUtf8("%1 new messages received").arg(countMessagesNewUnread));
        }
    }
}

void KulloTrayIcon::setIcon(IconState state)
{
    if (iconState_ == state) return;

#ifdef K_OS_WINDOWS
    static const auto defaultIcon      = QIcon(":/resources/scalable/trayicon_windows_default.svg");
    static const auto loggedOutIcon    = QIcon(":/resources/scalable/trayicon_windows_loggedout.svg");
    static const auto notificationIcon = QIcon(":/resources/scalable/trayicon_windows_notification.svg");
#elif K_OS_OSX
    static auto defaultIcon      = QIcon(":/resources/scalable/trayicon_osx_default.svg");
    static auto loggedOutIcon    = defaultIcon;
    static auto notificationIcon = QIcon(":/resources/scalable/trayicon_osx_notification.svg");
    defaultIcon.setIsMask(true);
    loggedOutIcon.setIsMask(true);
    notificationIcon.setIsMask(true);
#else
    static const auto defaultIcon      = QIcon(":/resources/scalable/trayicon_linux_orange.svg");
    static const auto loggedOutIcon    = QIcon(":/resources/scalable/trayicon_linux_gray.svg");
    static const auto notificationIcon = QIcon(":/resources/scalable/trayicon_linux_turquoise.svg");
#endif

    if (!state.loggedIn)
    {
        trayIcon_->setIcon(loggedOutIcon);
    }
    else
    {
        if (state.unreadMessages > 0)
        {
            trayIcon_->setIcon(notificationIcon);
        }
        else
        {
            trayIcon_->setIcon(defaultIcon);
        }
    }

    iconState_ = state;
}

}
}
