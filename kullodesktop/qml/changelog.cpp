/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#include "changelog.h"

#include <QRegularExpression>
#include <cpp-markdown/markdown.h>
#include <desktoputil/kulloclient2qt.h>
#include <kulloclient/util/librarylogger.h>

namespace {
const auto CHANGELOG_MD_URL = QStringLiteral("https://www.kullo.net/download/files/changelog.md");
//const auto CHANGELOG_MD_URL = QStringLiteral("http://localhost/download/files/changelog.md");
}

namespace KulloDesktop {
namespace Qml {

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
    QUrl url(CHANGELOG_MD_URL);

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

    auto changelogMd = DesktopUtil::KulloClient2Qt::fromUtf8(*downloadData_);
    markdown::Document md;
    md.read(changelogMd);
    std::ostringstream out;
    md.write(out);
    auto htmlContent = QString::fromStdString(out.str());
    htmlContent.remove(QRegularExpression("<h1>.*</h1>"));
    htmlContent = htmlContent.trimmed();
    changelogHtml_ = htmlContent;

    emit htmlChanged();
}

}
}
