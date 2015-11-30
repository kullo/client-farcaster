/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#include "kulloupdatermodel.h"

#include <QDir>
#include <QDirIterator>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QDesktopServices>
#include <desktoputil/qtypestreamers.h>
#include <kulloclient/util/assert.h>
#include <kulloclient/util/librarylogger.h>

#include "kullodesktop/qml/os.h"
#include "kullodesktop/util/paths.h"

namespace KulloDesktop {
namespace Qml {

KulloUpdaterModel::KulloUpdaterModel(QObject *parent)
    : QObject(parent)
{
    connect(&manager_, &DesktopUtil::AsyncHttpGetManager::downloadProgress,
            this, &KulloUpdaterModel::onDownloadProgress);
    connect(&manager_, &DesktopUtil::AsyncHttpGetManager::finished,
            this, &KulloUpdaterModel::onDownloadFinished);
}

KulloUpdaterModel::~KulloUpdaterModel()
{
}

bool KulloUpdaterModel::canRunInstaller() const
{
    if (Os::linux())
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool KulloUpdaterModel::installerNeedsClosedApp() const
{
    if (Os::osx() || Os::windows())
    {
        return true;
    }
    else
    {
        return false;
    }
}

void KulloUpdaterModel::run(const QUrl &url)
{
    if (!Os::linux())
    {
        cleanOldDownloads();
    }
    filename_ = url.fileName();
    filenameTmp_ = filename_ + ".part";
    downloadUrl(url, Util::kulloPaths().UPDATES_DIR, filenameTmp_);
}

void KulloUpdaterModel::cancel()
{
    manager_.cancel();
}

void KulloUpdaterModel::openTarget()
{
    QUrl target = canRunInstaller()
            ? QUrl::fromLocalFile(Util::kulloPaths().UPDATES_DIR + "/" + filename_)
            : QUrl::fromLocalFile(Util::kulloPaths().UPDATES_DIR);
    Log.i() << "Opening " << target;
    QDesktopServices::openUrl(target);
}

void KulloUpdaterModel::onDownloadProgress(quint64 bytesReceived, quint64 bytesTotal)
{
    if (bytesTotal == 0)
    {
        Log.w() << "KulloUpdaterModel::onDownloadProgress(): Total number of bytes unknown.";
        return;
    }
    if (bytesTotal < bytesReceived)
    {
        Log.e() << "KulloUpdaterModel::onDownloadProgress(): bytesReceived is bigger than bytesTotal.";
        return;
    }

    emit downloadProgress((100 * bytesReceived)/bytesTotal);
}

void KulloUpdaterModel::onDownloadFinished(DesktopUtil::AsyncHttpGetManager::Response response)
{
    file_->close();

    if (response->error)
    {
        switch (*(response->error)) {
        case DesktopUtil::AsyncHttpGetManager::ResponseError::NetworkError:
            Log.e() << "Network error";
            break;
        case DesktopUtil::AsyncHttpGetManager::ResponseError::Timeout:
            Log.e() << "Timeout";
            break;
        case DesktopUtil::AsyncHttpGetManager::ResponseError::Canceled:
            Log.i() << "Canceled";
            break;
        default:
            kulloAssert(false);
        }

        Log.i() << "Removing download fragment: " << file_->fileName();
        if (!file_->remove())
        {
            Log.e() << "Download fragment could not be removed: " << file_->fileName();
        }

        return;
    }

    postDownloadChecks();
}

void KulloUpdaterModel::onDownloadError()
{
    Log.e() << "Download error";
}

void KulloUpdaterModel::downloadUrl(const QUrl &url, const QString &dir, const QString &filename)
{
    Log.i() << "Downloading url " << url.toString() << " "
            << "to file " << filename << " in " << dir;

    QDir().mkpath(dir);
    file_ = std::make_shared<QFile>(dir + "/" + filename);
    file_->open(QIODevice::WriteOnly);

    manager_.get(url, file_);
}

void KulloUpdaterModel::cleanOldDownloads()
{
    QDirIterator it(Util::kulloPaths().UPDATES_DIR);
    while (it.hasNext())
    {
        QFileInfo fi(it.next());
        if (fi.isFile())
        {
            if (fi.absoluteFilePath().endsWith(".msi")
                    || fi.absoluteFilePath().endsWith(".msi.part")
                    || fi.absoluteFilePath().endsWith(".dmg")
                    || fi.absoluteFilePath().endsWith(".dmg.part"))
            {
                Log.i() << "Removing " << fi.absoluteFilePath();
                QFile::remove(fi.absoluteFilePath());
            }
        }
    }
}

void KulloUpdaterModel::postDownloadChecks()
{
    // TODO: check sha256 sum
    QDir::setCurrent(Util::kulloPaths().UPDATES_DIR);
    QFile::remove(filename_);
    QFile::rename(filenameTmp_, filename_);

    if (Os::linux())
    {
        QFlags<QFileDevice::Permission> permissions = QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ExeOwner;
        if (!QFile(filename_).setPermissions(permissions))
        {
            Log.w() << "Could not make Kullo download executeable: " << filename_ << " "
                    << "Current permissions:" << DesktopUtil::toString(QFile(filename_).permissions());
        }
    }

    Log.i() << "Update successfully downloaded to "
            << QFileInfo(Util::kulloPaths().UPDATES_DIR, filename_).absoluteFilePath();
    emit done();
}

}
}
