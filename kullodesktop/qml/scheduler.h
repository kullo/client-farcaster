/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <QObject>
#include <QTimer>
#include <kullodesktop/farcaster-forwards.h>

namespace KulloDesktop {
namespace Qml {

class Scheduler : public QObject
{
    Q_OBJECT

public:
    explicit Scheduler(QObject *parent = 0);
    ~Scheduler();

    Q_PROPERTY(KulloDesktop::Qml::UserSettingsModel* user READ user WRITE setUser NOTIFY userChanged)
    UserSettingsModel *user() const;
    void setUser(UserSettingsModel *user);

    Q_INVOKABLE void start();

signals:
    void userChanged();
    void showBackupReminderRequested();

private slots:
    // Runs the scheduler to check if something needs to be done.
    // This is a cheep operation as long as nothing is to be done.
    void run();

private:
    UserSettingsModel *user_;
    QTimer timer_;
};

}
}
