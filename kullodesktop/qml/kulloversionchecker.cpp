/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#include "kulloversionchecker.h"

#include <sstream>
#include <QByteArray>
#include <desktoputil/kulloversion.h>
#include <desktoputil/qtypestreamers.h>
#include <desktoputil/osdetection.h>
#include <desktoputil/versions.h>
#include <kulloclient/util/assert.h>
#include <kulloclient/util/version.h>
#include <kulloclient/util/librarylogger.h>

namespace {
const auto CHECK_URL_PATTERN = QStringLiteral("https://www.kullo.net/download/files/%1/latest-%2");
//const auto CHECK_URL_PATTERN = QStringLiteral("https://www.kullo.net/download/files/%1/latest-test-%2");
//const auto CHECK_URL_PATTERN = QStringLiteral("http://localhost/download/files/%1/latest-%2");

const auto UPDATE_URL = QStringLiteral("https://www.kullo.net/download/#%1");

const auto INTERVAL_CHECK = 5 * 60; // 5 minutes
const auto INTERVAL_RUN = 24 * 3600; // 24 hours

const auto OS_FAMILY = QStringLiteral(K_OS_FAMILY);
const auto DOWNLOAD_URL_PATTERN = QStringLiteral("https://www.kullo.net/download/files/%1/%2");

#if defined K_OS_WINDOWS
const auto DOWNLOAD_FILENAME_PATTERN = QStringLiteral("kullo-%1.msi");
#elif defined K_OS_OSX
const auto DOWNLOAD_FILENAME_PATTERN = QStringLiteral("Kullo-%1.dmg");
#elif defined K_OS_LINUX32
const auto DOWNLOAD_FILENAME_PATTERN = QStringLiteral("kullo-%1-linux32.run");
#elif defined K_OS_LINUX64
const auto DOWNLOAD_FILENAME_PATTERN = QStringLiteral("kullo-%1-linux64.run");
#else
#error "Unknown OS"
#endif
}

namespace KulloDesktop {
namespace Qml {

KulloVersionChecker::KulloVersionChecker(Applications::KulloApplication &app, QObject *parent)
    : QObject(parent)
    , app_(app)
{
    connect(&manager_, &DesktopUtil::AsyncHttpGetManager::finished,
            this, &KulloVersionChecker::onRequestFinished);
    connect(&timer_, &QTimer::timeout,
            this, &KulloVersionChecker::onMightNeedToRun);
    timer_.setTimerType(Qt::VeryCoarseTimer);
    timer_.start(INTERVAL_CHECK * 1000);

    connect(this, &KulloVersionChecker::versionAvailableChanged,
            this, &KulloVersionChecker::updateUrlChanged);
}

bool KulloVersionChecker::updateAvailable() const
{
    return !versionAvailable_.isEmpty();
}

QUrl KulloVersionChecker::updateUrl() const
{
    return QUrl(UPDATE_URL.arg(OS_FAMILY));
}

QString KulloVersionChecker::versionAvailable() const
{
    return versionAvailable_;
}

QUrl KulloVersionChecker::downloadUrl() const
{
    QString filename = DOWNLOAD_FILENAME_PATTERN.arg(versionAvailable_);
    QUrl url(DOWNLOAD_URL_PATTERN.arg(OS_FAMILY).arg(filename));
    return url;
}

void KulloVersionChecker::run()
{
    running_.lock();
    QUrl url(CHECK_URL_PATTERN.arg(OS_FAMILY, app_.deviceSettings().updateLane()));

    downloadData_ = std::make_shared<QByteArray>();
    downloadBuffer_ = std::make_shared<QBuffer>(downloadData_.get());
    downloadBuffer_->open(QIODevice::WriteOnly);

    manager_.get(url, downloadBuffer_);

    lastRunUtc_ = QDateTime::currentDateTimeUtc();
}

void KulloVersionChecker::onRequestFinished(DesktopUtil::AsyncHttpGetManager::Response response)
{
    if (response->error || response->statusCode != 200)
    {
        Log.e() << "Error getting latest Kullo version.";
        running_.unlock();
        return;
    }

    DesktopUtil::KulloVersion versionAvailable(QString::fromUtf8(*downloadData_).trimmed().toStdString());
    // All downloaded data processed, release mutex
    running_.unlock();

    DesktopUtil::KulloVersion versionInstalled = DesktopUtil::Versions::kullodesktopVersion();

    QString versionInstalledStr = QString::fromStdString(versionInstalled.toString());
    QString versionAvailableStr = QString::fromStdString(versionAvailable.toString());
    Log.i() << "Kullo version running: " << versionInstalledStr << " "
            << "available: "             << versionAvailableStr;

    if (versionAvailable > versionInstalled)
    {
        versionAvailable_ = versionAvailableStr;
        emit updateAvailableChanged();
        emit versionAvailableChanged();
        emit updateAvailable(versionInstalledStr, versionAvailableStr);
    }
}

void KulloVersionChecker::onMightNeedToRun()
{
    if (lastRunUtc_.isNull() || lastRunUtc_.secsTo(QDateTime::currentDateTimeUtc()) > INTERVAL_RUN)
    {
        run();
    }
}

}
}
