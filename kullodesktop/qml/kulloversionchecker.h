/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
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
    explicit KulloVersionChecker(InnerApplication &app, QObject *parent = 0);

    Q_PROPERTY(bool updateAvailable READ updateAvailable NOTIFY updateAvailableChanged)
    bool updateAvailable() const;

    Q_PROPERTY(QUrl updateUrl READ updateUrl NOTIFY updateUrlChanged)
    QUrl updateUrl() const;

    Q_PROPERTY(QString versionAvailable READ versionAvailable NOTIFY versionAvailableChanged)
    QString versionAvailable() const;

    Q_PROPERTY(QUrl downloadUrl READ downloadUrl NOTIFY downloadUrlChanged)
    QUrl downloadUrl() const;

    Q_INVOKABLE void run();

signals:
    void updateAvailable(const QString &versionInstalled, const QString &versionAvailable);
    void updateAvailableChanged();
    void versionAvailableChanged();
    void downloadUrlChanged();

    // unused signals
    void updateUrlChanged();

private slots:
    void onMightNeedToRun();
    void onRequestFinished(DesktopUtil::AsyncHttpGetManager::Response response);

private:
    void handleVersionRequestError();
    void handleVersionRequestSuccess(const QByteArray &returnBody);

    QTimer timer_;
    QDateTime lastSuccessfulRunUtc_;
    DesktopUtil::AsyncHttpGetManager manager_;
    QString versionAvailable_;
    InnerApplication &app_;

    std::shared_ptr<QByteArray> downloadData_;
    std::shared_ptr<QBuffer> downloadBuffer_;
    // Ensures that download data is not overwritten
    // before entirely processed.
    std::atomic<bool> running_;
};

}
}
