/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#include "uploader.h"

#include <QUrl>
#include <desktoputil/httpmultipart.h>
#include <desktoputil/kulloclient2qt.h>
#include <kulloclient/util/binary.h>
#include <kulloclient/util/librarylogger.h>

Uploader::Uploader(QObject *parent)
    : QObject(parent)
{
    connect(&manager_, &DesktopUtil::AsyncHttpPostManager::finished,
            this, &Uploader::onFinished);
}

Uploader::~Uploader()
{
}

void Uploader::upload(const Metadata &meta)
{
    auto url = QUrl(QString::fromStdString(meta.uploadUrl));

    DesktopUtil::HttpMultipart mp;
    mp.addTextPart("prod", meta.productName);
    mp.addTextPart("ver", meta.productVersion);
    mp.addTextPart("guid", meta.clientGuid);
    mp.addTextPart("ptime", meta.processUptime);
    mp.addTextPart("ctime", meta.cumulativeUptime);
    mp.addTextPart("email", meta.email);
    mp.addTextPart("comments", meta.comments);
    mp.addFilePart("upload_file_minidump", meta.dumpPath);

    manager_.post(url, mp.boundary(), Kullo::Util::to_vector(mp.toString()));
}

void Uploader::onFinished(DesktopUtil::AsyncHttpPostManager::Response response)
{
    if (response->error)
    {
        Log.e() << "An error occured";
        emit finished(false, "An error occured");
    }
    else
    {
        emit finished(true);
    }
}
