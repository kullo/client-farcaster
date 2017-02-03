/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#pragma once

#include <QObject>
#include <QTimer>
#include <kullodesktop/farcaster-forwards.h>

namespace KulloDesktop {
namespace QmlComponents {

class Scheduler : public QObject
{
    Q_OBJECT

public:
    explicit Scheduler(QObject *parent = 0);
    ~Scheduler();

    Q_PROPERTY(KulloDesktop::Qml::UserSettings* user READ user WRITE setUser NOTIFY userChanged)
    Qml::UserSettings *user() const;
    void setUser(Qml::UserSettings *user);

    Q_INVOKABLE void start();

signals:
    void userChanged();
    void showBackupReminderRequested();

private slots:
    // Runs the scheduler to check if something needs to be done.
    // This is a cheep operation as long as nothing is to be done.
    void run();

private:
    Qml::UserSettings *user_ = nullptr;
    QTimer timer_;
};

}
}
