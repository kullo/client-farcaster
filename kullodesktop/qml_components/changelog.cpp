/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#include "changelog.h"

#include <QRegularExpression>
#include <kulloclient/util/librarylogger.h>

namespace {
const auto CHANGELOG_URL_HTML = QStringLiteral("https://www.kullo.net/download/files/changelog.html");
}

namespace KulloDesktop {
namespace QmlComponents {

Changelog::Changelog(QObject *parent) : QObject(parent)
{
    connect(&manager_, &DesktopUtil::AsyncHttpGetManager::finished,
            this, &Changelog::onRequestFinished);
}

QString Changelog::html() const
{
    return changelogHtml_;
}

void Changelog::update()
{
    QUrl url(CHANGELOG_URL_HTML);

    downloadData_ = std::make_shared<QByteArray>();
    downloadBuffer_ = std::make_shared<QBuffer>(downloadData_.get());
    downloadBuffer_->open(QIODevice::WriteOnly);

    manager_.get(url, downloadBuffer_);
}

void Changelog::onRequestFinished(DesktopUtil::AsyncHttpGetManager::Response response)
{
    if (response->error || response->statusCode != 200)
    {
        Log.e() << "Error getting Changelog.";
        return;
    }

    auto htmlContent = QString::fromUtf8(*downloadData_);
    htmlContent.remove(QRegularExpression("<h1>.*</h1>"));
    htmlContent = htmlContent.trimmed();
    changelogHtml_ = htmlContent;

    emit htmlChanged();
}

}
}
