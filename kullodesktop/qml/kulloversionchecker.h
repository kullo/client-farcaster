/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#pragma once

#include <QBuffer>
#include <QDateTime>
#include <QObject>
#include <QUrl>
#include <QString>
#include <QTimer>
#include <atomic>
#include <desktoputil/asynchttpgetmanager.h>

#include "kullodesktop/farcaster-forwards.h"

namespace KulloDesktop {
namespace Qml {

class KulloVersionChecker : public QObject
{
    Q_OBJECT

public:
    explicit KulloVersionChecker(InnerApplication &app, QObject *parent = nullptr);

    Q_PROPERTY(bool updateAvailable READ updateAvailable NOTIFY updateAvailableChanged)
    bool updateAvailable() const;

    Q_PROPERTY(QUrl updateUrl READ updateUrl NOTIFY updateUrlChanged)
    QUrl updateUrl() const;

    Q_PROPERTY(QString versionAvailable READ versionAvailable NOTIFY versionAvailableChanged)
    QString versionAvailable() const;

    Q_PROPERTY(QUrl downloadUrl READ downloadUrl NOTIFY downloadUrlChanged)
    QUrl downloadUrl() const;

    Q_PROPERTY(QDateTime lastSuccessfulCheck READ lastSuccessfulCheck NOTIFY lastSuccessfulCheckChanged)
    QDateTime lastSuccessfulCheck() const;

    // The internal timer starts automatically. This method can be used to trigger a manual check
    Q_INVOKABLE void run();

signals:
    void updateNotificationReceived(const QString &versionInstalled, const QString &versionAvailable);
    void updateAvailableChanged();
    void versionAvailableChanged();
    void downloadUrlChanged();
    void lastSuccessfulCheckChanged();

    // unused signals
    void updateUrlChanged();

private slots:
    void onMightNeedToRun();
    void onRequestFinished(DesktopUtil::AsyncHttpGetManager::Response response);

private:
    void handleVersionRequestError();
    void handleVersionRequestSuccess(const QByteArray &returnBody);

    QTimer timer_;
    QDateTime lastSuccessfulCheckUtc_;
    DesktopUtil::AsyncHttpGetManager manager_;
    QString versionAvailable_;
    bool updateAvailable_ = false;
    InnerApplication &app_;

    std::shared_ptr<QByteArray> downloadData_;
    std::shared_ptr<QBuffer> downloadBuffer_;
    // Ensures that download data is not overwritten
    // before entirely processed.
    std::atomic<bool> running_;
};

}
}
