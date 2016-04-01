/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#include "kullotrayicon.h"

#include <QAction>
#include <QApplication>
#include <QMenu>
#include <QSystemTrayIcon>
#include <QWindow>

#include <kulloclient/util/assert.h>
#include <kulloclient/util/librarylogger.h>
#include <kulloclient/util/misc.h>

#include <desktoputil/osdetection.h>

#include "kullodesktop/applications/kulloapplication.h"
#include "kullodesktop/qml/conversationlistsource.h"

namespace KulloDesktop {
namespace OsIntegration {

namespace {

const auto ICON_STATE_LOGGEDOUT = IconState(false, 0);

}

KulloTrayIcon::KulloTrayIcon(Applications::KulloApplication &app, QWindow &mainWindow, Qml::ClientModel& clientModel, QObject *parent)
    : QObject(parent)
    , iconState_(IconState(false, 42))  // != _LOGGEDOUT, so that setIcon works
    , app_(app)
    , mainWindow_(mainWindow)
    , clientModel_(clientModel)
    , trayIcon_(Kullo::make_unique<QSystemTrayIcon>(nullptr))
{
    trayIcon_->setToolTip(app_.applicationName());
    onLoggedInChanged(clientModel_.loggedIn());

    if (QSystemTrayIcon::supportsMessages())
    {
        Log.i() << "Tray icon messages supported.";
    }
    else
    {
        Log.i() << "Tray icon messages not supported.";
    }

    // Restore when application requests it (e.g. OS X dock click)
    connect(&app_, &Applications::KulloApplication::showMainWindowRequested,
            this, &KulloTrayIcon::showMainWindow);

    if (DesktopUtil::OsDetection::windows() || DesktopUtil::OsDetection::linux())
    {
        // Restore on icon click
        connect(trayIcon_.get(), &QSystemTrayIcon::activated,
                [this](QSystemTrayIcon::ActivationReason reason)
        {
            if (reason == QSystemTrayIcon::Trigger)
            {
                this->showMainWindow();
            }
        });
    }

    if (!DesktopUtil::OsDetection::windows())
    {
        // Restore on context menu click
        trayIconMenuActions_.emplace_back(Kullo::make_unique<QAction>(trUtf8("Show"), nullptr));
        connect(trayIconMenuActions_.back().get(), &QAction::triggered,
                this, &KulloTrayIcon::showMainWindow);
    }

    trayIconMenuActions_.emplace_back(Kullo::make_unique<QAction>(trUtf8("Quit"), nullptr));
    connect(trayIconMenuActions_.back().get(), &QAction::triggered, [this]()
    {
        Log.i() << "Quit now!";
        app_.quit();
    });

    trayIconMenu_ = Kullo::make_unique<QMenu>();
    for (auto &action : trayIconMenuActions_)
    {
        trayIconMenu_->addAction(action.get());
    }
    trayIcon_->setContextMenu(trayIconMenu_.get());

    connect(trayIcon_.get(), &QSystemTrayIcon::messageClicked,
            this, &KulloTrayIcon::showMainWindow);

    connect(&clientModel_, &Qml::ClientModel::syncFinished,
            [this](bool success, int countMessagesNew, int countMessagesNewUnread, int countMessagesModified, int countMessagesDeleted)
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
    });

    connect(&clientModel_, &Qml::ClientModel::loggedInChanged,
            this, &KulloTrayIcon::onLoggedInChanged);

    trayIcon_->show();
}

KulloTrayIcon::~KulloTrayIcon()
{
}

void KulloTrayIcon::onLoggedInChanged(bool loggedIn)
{
    if (loggedIn)
    {
        kulloAssert(clientModel_.conversationsListSource());

        auto update = [this]()
        {
            const auto unreadMessagesCount = clientModel_.conversationsListSource()->unreadMessagesCount();
            Log.d() << "Unread message count changed: " << unreadMessagesCount;
            setIcon(IconState(true, unreadMessagesCount));
        };

        connect(clientModel_.conversationsListSource().get(),
                &Qml::ConversationListSource::unreadMessagesCountChanged,
                update);
        update();
    }
    else
    {
        setIcon(ICON_STATE_LOGGEDOUT);
    }
}

void KulloTrayIcon::showMainWindow()
{
    Log.d() << "Show main window now!";

    if (mainWindow_.windowState() == Qt::WindowMinimized)
    {
        mainWindow_.showNormal();
    }
    else
    {
        mainWindow_.show();
    }
    mainWindow_.raise();
}

void KulloTrayIcon::setIcon(IconState state)
{
    if (iconState_ == state) return;

#ifdef K_OS_WINDOWS
    static const auto defaultIcon      = QIcon(":/resources/scalable/trayicon_windows_default.svg");
    static const auto loggedOutIcon    = QIcon(":/resources/scalable/trayicon_windows_loggedout.svg");
    static const auto notificationIcon = QIcon(":/resources/scalable/trayicon_windows_notification.svg");
#elif K_OS_OSX
    static const auto defaultIcon      = QIcon(":/resources/scalable/trayicon_osx_default.svg");
    static const auto loggedOutIcon    = QIcon(":/resources/scalable/trayicon_osx_loggedout.svg");
    static const auto notificationIcon = QIcon(":/resources/scalable/trayicon_osx_notification.svg");
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
