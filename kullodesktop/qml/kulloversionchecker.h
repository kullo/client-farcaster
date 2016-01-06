/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <QBuffer>
#include <QDateTime>
#include <QObject>
#include <QUrl>
#include <QString>
#include <QTimer>
#include <mutex>
#include <desktoputil/asynchttpgetmanager.h>

#include "kullodesktop/applications/kulloapplication.h"

namespace KulloDesktop {
namespace Qml {

class KulloVersionChecker : public QObject
{
    Q_OBJECT

public:
    explicit KulloVersionChecker(Applications::KulloApplication &app, QObject *parent = 0);

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
    void onRequestFinished(DesktopUtil::AsyncHttpGetManager::Response response);
    void onMightNeedToRun();

private:
    QTimer timer_;
    QDateTime lastRunUtc_;
    DesktopUtil::AsyncHttpGetManager manager_;
    QString versionAvailable_;
    Applications::KulloApplication &app_;

    std::shared_ptr<QByteArray> downloadData_;
    std::shared_ptr<QBuffer> downloadBuffer_;
    // Ensures that download data is not overwritten
    // before entirely processed.
    std::mutex running_;
};

}
}
