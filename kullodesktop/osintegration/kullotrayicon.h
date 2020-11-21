/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#pragma once

#include <QObject>
#include <memory>
#include <vector>

#include "kullodesktop/farcaster-forwards.h"

class QAction;
class QMenu;
class QSystemTrayIcon;

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
    explicit KulloTrayIcon(Qml::InnerApplication& app,
                           QObject *parent = nullptr);
    ~KulloTrayIcon();

signals:
    void showMainWindowRequested();

public slots:
    void onHasSessionChanged(bool hasSession);
    void onUnreadMessagesCountChanged(int count);
    void onSyncFinished(bool success,
                        int countMessagesNew = 0,
                        int countMessagesNewUnread = 0,
                        int countMessagesModified = 0,
                        int countMessagesDeleted = 0);

private:
    void setIcon(IconState state);

    IconState iconState_;
    Qml::InnerApplication& app_;
    std::vector<std::unique_ptr<QAction>> trayIconMenuActions_;
    std::unique_ptr<QMenu> trayIconMenu_;
    std::unique_ptr<QSystemTrayIcon> trayIcon_;
};

}
}
