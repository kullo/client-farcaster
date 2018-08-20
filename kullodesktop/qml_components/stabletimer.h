/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
#pragma once

#include <QObject>
#include <QTimer>

namespace KulloDesktop {
namespace QmlComponents {

// A Timer that aims to be a drop-in replacement for the QML Timer
class StableTimer : public QObject
{
    Q_OBJECT

public:
    explicit StableTimer(QObject *parent = 0);

    // defaults to 1000
    Q_PROPERTY(int interval READ interval WRITE setInterval NOTIFY intervalChanged)
    int interval() const;
    void setInterval(const int msec);

    // defaults to false
    Q_PROPERTY(bool repeat READ repeat WRITE setRepeat NOTIFY repeatChanged)
    bool repeat() const;
    void setRepeat(const bool repeat);

    // defaults to false
    Q_PROPERTY(bool running READ running WRITE setRunning NOTIFY runningChanged)
    bool running() const;
    void setRunning(const bool running);

    // defaults to false
    Q_PROPERTY(bool triggeredOnStart READ triggeredOnStart WRITE setTriggeredOnStart NOTIFY triggeredOnStartChanged)
    bool triggeredOnStart() const;
    void setTriggeredOnStart(const bool triggeredOnStart);

    Q_INVOKABLE void restart();
    Q_INVOKABLE void start();
    Q_INVOKABLE void stop();

signals:
    void intervalChanged();
    void repeatChanged();
    void runningChanged();
    void triggeredOnStartChanged();

    void triggered();

private:
    void preStartActions();

    QTimer impl_;
    bool triggeredOnStart_ = false;
};

}
}
