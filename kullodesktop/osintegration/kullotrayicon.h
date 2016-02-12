/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <QObject>
#include <memory>
#include <vector>

#include "kullodesktop/applications/kulloapplication.h"
#include "kullodesktop/qml/clientmodel.h"

class QAction;
class QApplication;
class QMenu;
class QSystemTrayIcon;
class QWindow;

namespace KulloDesktop {
namespace OsIntegration {

struct IconState {
    bool loggedIn;
    quint32 unreadMessages;

    IconState(bool pLoggedIn, quint32 pUnreadMessages)
        : loggedIn(pLoggedIn)
        , unreadMessages(pUnreadMessages)
    {}

    bool operator==(const IconState &other)
    {
        return loggedIn == other.loggedIn
                && unreadMessages == other.unreadMessages;
    }
};

class KulloTrayIcon : public QObject
{
    Q_OBJECT

public:
    explicit KulloTrayIcon(Applications::KulloApplication& app, QWindow& mainWindow, Qml::ClientModel& clientModel, QObject *parent = 0);
    ~KulloTrayIcon();

private slots:
    void onLoggedInChanged(bool loggedIn);
    void showMainWindow();

private:
    void setIcon(IconState state);

    IconState iconState_;
    Applications::KulloApplication& app_;
    QWindow& mainWindow_;
    Qml::ClientModel& clientModel_;
    std::vector<std::unique_ptr<QAction>> trayIconMenuActions_;
    std::unique_ptr<QMenu> trayIconMenu_;
    std::unique_ptr<QSystemTrayIcon> trayIcon_;
};

}
}
