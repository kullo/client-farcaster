/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <QObject>
#include <QString>
#include <memory>

class QLocalServer;
class QLocalSocket;

namespace KulloDesktop {
namespace OsIntegration {

class SingleInstanceLock : public QObject
{
    Q_OBJECT

public:
    enum class AcquireResult {
        InformedOtherInstance,
        CreatedInstance,
    };

    explicit SingleInstanceLock(const QString &name, QObject *parent = nullptr);
    ~SingleInstanceLock();

    SingleInstanceLock::AcquireResult acquire();
    void release();

signals:
    void showMainWindowRequested();

private:
    std::unique_ptr<QLocalSocket> connectToServer();
    void purgeBrokenServer();
    void startServer();

    const QString name_;
    std::unique_ptr<QLocalServer> server_;

    Q_DISABLE_COPY(SingleInstanceLock)
};

}
}
