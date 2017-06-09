/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#include "attachmentmodel.h"

#include <boost/optional.hpp>
#include <boost/optional/optional_io.hpp>
#include <QDesktopServices>
#include <QDir>
#include <QUrl>

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

#include "kullodesktop/applications/kulloapplication.h"

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

bool AttachmentModel::saveToAsync(const QUrl &url)
{
    if (!url.isLocalFile())
    {
        Log.e() << "Destination URL " << url << " is not a local file.";
        return false;
    }

    return doSaveTo(url.toLocalFile().toStdString(), [](bool){});
}

bool AttachmentModel::openAsync()
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

    const auto startedOk = doSaveTo(tmpFileInfo.absoluteFilePath().toStdString(), [=](bool success) {
        if (success) {
            Log.i() << "Opening file " << tmpFileInfo.absoluteFilePath();
            QUrl tmpFileUrl = QUrl::fromLocalFile(tmpFileInfo.absoluteFilePath());
            QDesktopServices::openUrl(tmpFileUrl);
        }
    });
    return startedOk;
}

Kullo::id_type AttachmentModel::messageId() const
{
    return msgId_;
}

void AttachmentModel::pruneDoneTasks()
{
    if (saveToTask_ && saveToTask_->isDone())
    {
        saveToTask_ = nullptr;
    }
}

bool AttachmentModel::doSaveTo(const std::string &path, const std::function<void (bool /* success */)> &callback)
{
    pruneDoneTasks();
    if (saveToTask_) return false; // only one save to action is allowed at the same time

    saveToListener_ = std::make_shared<ApiMirror::MessageAttachmentsSaveToListener>();
    connect(saveToListener_.get(), &ApiMirror::MessageAttachmentsSaveToListener::_error,
            [=](int64_t msgId, int64_t attId, const std::string &path, Kullo::Api::LocalError error)
    {
        K_UNUSED(msgId);
        K_UNUSED(attId);
        Log.e() << "File could not be saved to " << path << ", error " << error;

        Applications::KulloApplication::runOnMainThread([=]() {
            callback(false);
        });
    });

    connect(saveToListener_.get(), &ApiMirror::MessageAttachmentsSaveToListener::_finished,
            [=](Kullo::id_type msgId, Kullo::id_type attId, const std::string &path)
    {
        K_UNUSED(msgId);
        K_UNUSED(attId);
        K_UNUSED(path);
        Applications::KulloApplication::runOnMainThread([=]() {
            callback(true);
        });
    });

    saveToTask_ = session_->messageAttachments()->saveToAsync(msgId_, attId_, path, saveToListener_);
    return true; // started successfully, may result in error
}

}
}
