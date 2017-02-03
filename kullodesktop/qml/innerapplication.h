/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#pragma once

#include <QObject>

#include <desktoputil/databasefiles.h>

#include "kullodesktop/farcaster-forwards.h"
#include "kullodesktop/qml/devicesettings.h"

namespace KulloDesktop {
namespace Qml {

/*
 * Application should not depend in ClientModel, since it lives longer than the ClientModel.
 * Instead, let ClientModel pass all relevant information via signal/slot to application.
 */
class InnerApplication : public QObject
{
    Q_OBJECT

public:
    explicit InnerApplication(Applications::KulloApplication &mainApplication, QObject *parent = 0);
    ~InnerApplication();

    Q_PROPERTY(KulloDesktop::Qml::DeviceSettings *deviceSettings READ deviceSettings NOTIFY deviceSettingsChanged)
    DeviceSettings *deviceSettings();

    const DesktopUtil::DatabaseFiles &databaseFiles() const;
    Applications::KulloApplication &mainApplication();

    void setMainWindow(QWindow *mainWindow);

signals:
    void loggedInChanged(bool loggedIn);
    void unreadMessagesCountChanged(int count);
    void syncFinished(bool success,
                      int countMessagesNew = 0,
                      int countMessagesNewUnread = 0,
                      int countMessagesModified = 0,
                      int countMessagesDeleted = 0);

    // unused
    void deviceSettingsChanged();

public slots:
    void onLoggedInChanged(bool loggedIn);
    void onUnreadMessagesCountChanged(int count);
    void showMainWindowIfPossible();

private:
    void setCloseToTray(bool closeToTray);

    Applications::KulloApplication &mainApplication_;
    const DesktopUtil::DatabaseFiles databaseFiles_;
    Qml::DeviceSettings settings_;

    QWindow *mainWindow_ = nullptr;
    std::unique_ptr<OsIntegration::KulloTrayIcon> icon_;
};

}
}
