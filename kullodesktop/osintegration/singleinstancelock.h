/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
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
