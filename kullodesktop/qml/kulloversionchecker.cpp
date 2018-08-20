/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
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

#include "kullodesktop/qml/innerapplication.h"

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

KulloVersionChecker::KulloVersionChecker(InnerApplication &app, QObject *parent)
    : QObject(parent)
    , app_(app)
    , running_(false)
{
    connect(&manager_, &DesktopUtil::AsyncHttpGetManager::finished,
            this, &KulloVersionChecker::onRequestFinished);
    connect(this, &KulloVersionChecker::versionAvailableChanged,
            this, &KulloVersionChecker::updateUrlChanged);

    // setup timer
    connect(&timer_, &QTimer::timeout,
            this, &KulloVersionChecker::onMightNeedToRun);

    timer_.setTimerType(Qt::VeryCoarseTimer);
    timer_.setInterval(INTERVAL_CHECK * 1000);

    // start timer delayed (to avoid lags during ui loading and session creation)
    QTimer::singleShot(3000, [this]() {
        run(); // timer does not fire on start
        timer_.start();
    });
}

bool KulloVersionChecker::updateAvailable() const
{
    return updateAvailable_;
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

QDateTime KulloVersionChecker::lastSuccessfulCheck() const
{
    return lastSuccessfulCheckUtc_.toLocalTime();
}

void KulloVersionChecker::run()
{
    if (running_.exchange(true))
    {
        // running_ was already true
        return;
    }

    QUrl url(CHECK_URL_PATTERN.arg(OS_FAMILY, app_.deviceSettings()->updateLane()));

    downloadData_ = std::make_shared<QByteArray>();
    downloadBuffer_ = std::make_shared<QBuffer>(downloadData_.get());
    downloadBuffer_->open(QIODevice::WriteOnly);

    manager_.get(url, downloadBuffer_);
}

void KulloVersionChecker::onMightNeedToRun()
{
    const auto nowUtc = QDateTime::currentDateTimeUtc();
    if (lastSuccessfulCheckUtc_.isNull() || lastSuccessfulCheckUtc_.secsTo(nowUtc) > INTERVAL_RUN)
    {
        run();
    }
}

void KulloVersionChecker::onRequestFinished(DesktopUtil::AsyncHttpGetManager::Response response)
{
    if (response->error || response->statusCode != 200)
    {
        handleVersionRequestError();
    }
    else
    {
        handleVersionRequestSuccess(*downloadData_);
    }
    running_ = false;
}

void KulloVersionChecker::handleVersionRequestError()
{
    Log.e() << "Error getting latest Kullo version.";
}

void KulloVersionChecker::handleVersionRequestSuccess(const QByteArray &returnBody)
{
    lastSuccessfulCheckUtc_ = QDateTime::currentDateTimeUtc();
    emit lastSuccessfulCheckChanged();

    DesktopUtil::KulloVersion versionAvailable(QString::fromUtf8(returnBody).trimmed().toStdString());

    DesktopUtil::KulloVersion versionInstalled = DesktopUtil::Versions::kullodesktopVersion();

    QString versionInstalledStr = QString::fromStdString(versionInstalled.toString());
    QString versionAvailableStr = QString::fromStdString(versionAvailable.toString());
    Log.i() << "Kullo version running: " << versionInstalledStr << " "
            << "available: "             << versionAvailableStr;

    versionAvailable_ = versionAvailableStr;
    emit versionAvailableChanged();

    updateAvailable_ = (versionAvailable > versionInstalled);
    emit updateAvailableChanged();

    if (updateAvailable_)
    {
        emit updateNotificationReceived(versionInstalledStr, versionAvailableStr);
    }
}

}
}
