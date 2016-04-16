/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#include "attachmentmodel.h"

#include <boost/optional.hpp>
#include <boost/optional/optional_io.hpp>
#include <QDesktopServices>
#include <QDir>
#include <QUrl>

#include <apimirror/MessageAttachmentsContentListener.h>
#include <apimirror/MessageAttachmentsSaveToListener.h>
#include <desktoputil/filesystem.h>
#include <desktoputil/qtypestreamers.h>
#include <desktoputil/stlqt.h>
#include <kulloclient/api/AsyncTask.h>
#include <kulloclient/api/MessageAttachments.h>
#include <kulloclient/api_impl/debug.h>
#include <kulloclient/util/assert.h>
#include <kulloclient/util/librarylogger.h>
#include <kulloclient/util/misc.h>

namespace KulloDesktop {
namespace Qml {

AttachmentModel::AttachmentModel(QObject *parent)
    : QObject(parent)
{
    Log.f() << "Don't instantiate Attachment in QML.";
}

AttachmentModel::AttachmentModel(
        const std::shared_ptr<Kullo::Api::Session> &session,
        Kullo::id_type msgId,
        Kullo::id_type attId,
        QObject *parent)
    : QObject(parent)
    , session_(session)
    , msgId_(msgId)
    , attId_(attId)
{
    kulloAssert(session_);
}

QString AttachmentModel::filename() const
{
    return QString::fromStdString(session_->messageAttachments()->filename(msgId_, attId_));
}

QString AttachmentModel::hash() const
{
    return QString::fromStdString(session_->messageAttachments()->hash(msgId_, attId_));
}

QString AttachmentModel::mimeType() const
{
    return QString::fromStdString(session_->messageAttachments()->mimeType(msgId_, attId_));
}

quint32 AttachmentModel::size() const
{
    return session_->messageAttachments()->size(msgId_, attId_);
}

//TODO make AttachmentModel::saveTo asynchronous
bool AttachmentModel::saveTo(const QUrl &url) const
{
    if (!url.isLocalFile())
    {
        Log.e() << "Destination URL " << url << " is not a local file.";
        return false;
    }

    return doSaveTo(url.toLocalFile().toStdString());
}

//TODO make AttachmentModel::open asynchronous
bool AttachmentModel::open() const
{
    QString tmpFilename = DesktopUtil::Filesystem::prepareTmpFilename(filename());

    QString tmpPath = QStringLiteral("%1/kullo/attachments/%2/%3")
            .arg(QDir::tempPath())
            .arg(messageId())
            .arg(tmpFilename);

    Log.i() << "Save attachment to temporary file: " << tmpPath;

    QFileInfo tmpFileInfo(tmpPath);
    // TODO: Make check compatible with symbolic links
    if (!tmpFileInfo.absoluteDir().exists())
    {
        Log.i() << "Creating attachments directory: " << tmpFileInfo.absolutePath();
        if (!QDir().mkpath(tmpFileInfo.absolutePath()))
        {
            Log.e() << "Could not create attachments directory: " << tmpFileInfo.absolutePath();
            return false;
        }
    }
    if (!QFileInfo(tmpFileInfo.absolutePath()).isWritable())
    {
        Log.e() << "Attachments directory not writeable: " << tmpFileInfo.absolutePath();
        return false;
    }

    if (!doSaveTo(tmpFileInfo.absoluteFilePath().toStdString())) return false;

    Log.i() << "Opening file " << tmpFileInfo.absoluteFilePath();
    QUrl tmpFileUrl = QUrl::fromLocalFile(tmpFileInfo.absoluteFilePath());
    return QDesktopServices::openUrl(tmpFileUrl);
}

QByteArray AttachmentModel::content() const
{
    auto listener = std::make_shared<ApiMirror::MessageAttachmentsContentListener>();
    std::vector<unsigned char> data;
    connect(listener.get(), &ApiMirror::MessageAttachmentsContentListener::_finished,
            [&data](int64_t msgId, int64_t attId, const std::vector<uint8_t> &content)
    {
        K_UNUSED(msgId);
        K_UNUSED(attId);
        data = content;
    });

    // Synchronous operation is no problem here, as content() is only used for
    // ImageProviders which operate asynchronously.
    session_->messageAttachments()->contentAsync(msgId_, attId_, listener)->waitUntilDone();
    return DesktopUtil::StlQt::toQByteArray(data);
}

Kullo::id_type AttachmentModel::messageId() const
{
    return msgId_;
}

bool AttachmentModel::doSaveTo(const std::string &path) const
{
    auto listener = std::make_shared<ApiMirror::MessageAttachmentsSaveToListener>();
    boost::optional<Kullo::Api::LocalError> saveErr;
    connect(listener.get(), &ApiMirror::MessageAttachmentsSaveToListener::_error,
            [&saveErr](int64_t msgId, int64_t attId, const std::string &path, Kullo::Api::LocalError error)
    {
        K_UNUSED(msgId);
        K_UNUSED(attId);
        K_UNUSED(path);
        saveErr = error;
    });

    session_->messageAttachments()->saveToAsync(msgId_, attId_, path, listener)->waitUntilDone();

    if (saveErr)
    {
        Log.e() << "File could not be saved to " << path << ", error " << saveErr;
        return false;
    }
    return true;
}

}
}
