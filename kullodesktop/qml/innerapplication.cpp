/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#include "innerapplication.h"

#include <QQmlEngine>
#include <QWindow>

#include <kulloclient/util/assert.h>
#include <kulloclient/util/librarylogger.h>

#include <desktoputil/paths.h>

#include "kullodesktop/applications/kulloapplication.h"
#include "kullodesktop/osintegration/kullotrayicon.h"

namespace KulloDesktop {
namespace Qml {

namespace {
bool runningUnderGnome()
{
    auto desktop = std::getenv("DESKTOP_SESSION");
    const std::string prefix = "gnome";
    return desktop && !std::strncmp(desktop, prefix.data(), prefix.length());
}
}

InnerApplication::InnerApplication(Applications::KulloApplication &mainApplication,
                                   QObject *parent)
    : QObject(parent)
    , mainApplication_(mainApplication)
    , databaseFiles_(DesktopUtil::kulloPaths().DATABASE_DIR + QStringLiteral("/libkulloclient-%1.db"))
{
    // Restore when application requests it (e.g. OS X dock click)
    connect(&mainApplication_, &Applications::KulloApplication::showMainWindowRequested,
            this, &InnerApplication::showMainWindowIfPossible);

    if (Applications::KulloApplication::NO_TRAY_ICON)
    {
        setCloseToTray(false);
    }
    else
    {
        setCloseToTray(settings_.closeToTray());
        connect(&settings_, &Qml::DeviceSettings::closeToTrayChanged,
                this, [this](bool value)
        {
            this->setCloseToTray(value);
        });
    }
}

InnerApplication::~InnerApplication()
{
}

DeviceSettings *InnerApplication::deviceSettings()
{
    auto out = &settings_;
    QQmlEngine::setObjectOwnership(out, QQmlEngine::CppOwnership);
    return out;
}

const DesktopUtil::DatabaseFiles &InnerApplication::databaseFiles() const
{
    return databaseFiles_;
}

Applications::KulloApplication &InnerApplication::mainApplication()
{
    return mainApplication_;
}

void InnerApplication::setMainWindow(QWindow *mainWindow)
{
    mainWindow_ = mainWindow;
}

void InnerApplication::onLoggedInChanged(bool loggedIn)
{
    emit loggedInChanged(loggedIn);
}

void InnerApplication::onUnreadMessagesCountChanged(int count)
{
    emit unreadMessagesCountChanged(count);
}

void InnerApplication::showMainWindowIfPossible()
{
    if (!mainWindow_) return;

    Log.d() << "Show main window now!";

    if (mainWindow_->windowState() == Qt::WindowMinimized)
    {
        mainWindow_->showNormal();
    }
    else
    {
        mainWindow_->show();
    }
    mainWindow_->raise();
}

void InnerApplication::setCloseToTray(bool closeToTray)
{
    mainApplication_.setQuitOnLastWindowClosed(!closeToTray);

    if (!closeToTray && runningUnderGnome())
    {
        // Hide icon if we don't close to tray, because otherwise the app won't
        // quit when the last window is closed.
        icon_.reset();
    }
    else if (!icon_)
    {
        icon_ = Kullo::make_unique<OsIntegration::KulloTrayIcon>(*this, nullptr);

        // app -> icon
        connect(this, &InnerApplication::loggedInChanged,
                icon_.get(), &OsIntegration::KulloTrayIcon::onLoggedInChanged);
        connect(this, &InnerApplication::unreadMessagesCountChanged,
                icon_.get(), &OsIntegration::KulloTrayIcon::onUnreadMessagesCountChanged);
        connect(this, &InnerApplication::syncFinished,
                icon_.get(), &OsIntegration::KulloTrayIcon::onSyncFinished);

        // icon -> app
        connect(icon_.get(), &OsIntegration::KulloTrayIcon::showMainWindowRequested,
                this, &InnerApplication::showMainWindowIfPossible);

        Log.i() << "Done building tray icon.";
    }
}

}
}
