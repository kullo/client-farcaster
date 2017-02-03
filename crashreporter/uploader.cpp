/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#include "uploader.h"

#include <QUrl>
#include <desktoputil/kulloclient2qt.h>
#include <kulloclient/util/binary.h>
#include <kulloclient/util/librarylogger.h>
#include <kulloclient/util/mimemultipart.h>

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

    Kullo::Util::MimeMultipart mp;
    mp.addPart("prod", meta.productName);
    mp.addPart("ver", meta.productVersion);
    mp.addPart("guid", meta.clientGuid);
    mp.addPart("ptime", meta.processUptime);
    mp.addPart("ctime", meta.cumulativeUptime);
    mp.addPart("email", meta.email);
    mp.addPart("comments", meta.comments);
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
