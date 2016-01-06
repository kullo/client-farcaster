/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <memory>
#include <QObject>
#include <QFile>
#include <desktoputil/asynchttpgetmanager.h>

namespace KulloDesktop {
namespace Qml {

class KulloUpdaterModel : public QObject
{
    Q_OBJECT

public:
    explicit KulloUpdaterModel(QObject *parent = 0);
    ~KulloUpdaterModel();

    Q_PROPERTY(bool canRunInstaller READ canRunInstaller NOTIFY canRunInstallerChanged)
    bool canRunInstaller() const;

    Q_PROPERTY(bool installerNeedsClosedApp READ installerNeedsClosedApp NOTIFY installerNeedsClosedAppChanged)
    bool installerNeedsClosedApp() const;

    Q_INVOKABLE void run(const QUrl &url);
    Q_INVOKABLE void cancel();
    Q_INVOKABLE void openTarget();

signals:
    void downloadProgress(int percentage);
    void done();

    // unused
    void canRunInstallerChanged();
    void installerNeedsClosedAppChanged();

private slots:
    void onDownloadProgress(quint64 bytesReceived, quint64 bytesTotal);
    void onDownloadFinished(DesktopUtil::AsyncHttpGetManager::Response response);
    void onDownloadError();

private:
    void downloadUrl(const QUrl &url, const QString &dir, const QString &filename);
    void cleanOldDownloads();
    void postDownloadChecks();

    DesktopUtil::AsyncHttpGetManager manager_;
    std::shared_ptr<QFile> file_;
    QString filename_;
    QString filenameTmp_;
};

}
}
