/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#include "draftattachmentmodel.h"

#include <QDesktopServices>
#include <QDir>
#include <QFileInfo>
#include <QUrl>

#include <apimirror/DraftAttachmentsSaveToListener.h>
#include <desktoputil/filesystem.h>
#include <desktoputil/stlqt.h>
#include <desktoputil/qtypestreamers.h>
#include <kulloclient/api/AsyncTask.h>
#include <kulloclient/api/DraftAttachments.h>
#include <kulloclient/api/Session.h>
#include <kulloclient/api_impl/debug.h>
#include <kulloclient/util/assert.h>
#include <kulloclient/util/librarylogger.h>
#include <kulloclient/util/misc.h>

#include "kullodesktop/applications/kulloapplication.h"

namespace KulloDesktop {
namespace Qml {

DraftAttachmentModel::DraftAttachmentModel(QObject *parent)
    : QObject(parent)
{
    Log.f() << "Don't instantiate DraftAttachment in QML.";
}

DraftAttachmentModel::DraftAttachmentModel(
        const std::shared_ptr<Kullo::Api::Session> &session,
        Kullo::id_type convId,
        Kullo::id_type attId,
        QObject *parent)
    : QObject(parent)
    , session_(session)
    , convId_(convId)
    , attId_(attId)
{
    kulloAssert(session_);
}

QString DraftAttachmentModel::filename() const
{
    return QString::fromStdString(session_->draftAttachments()->filename(convId_, attId_));
}

Kullo::id_type DraftAttachmentModel::index() const
{
    return attId_;
}

QString DraftAttachmentModel::mimeType() const
{
    return QString::fromStdString(session_->draftAttachments()->mimeType(convId_, attId_));
}

QString DraftAttachmentModel::hash() const
{
    return QString::fromStdString(session_->draftAttachments()->hash(convId_, attId_));
}

Kullo::id_type DraftAttachmentModel::size() const
{
    return session_->draftAttachments()->size(convId_, attId_);
}

void DraftAttachmentModel::deletePermanently()
{
    session_->draftAttachments()->remove(convId_, attId_);
}

bool DraftAttachmentModel::openAsync()
{
    QString tmpFilename = DesktopUtil::Filesystem::prepareTmpFilename(filename());

    QString tmpPath = QStringLiteral("%1/kullo/draftattachments/%2")
            .arg(QDir::tempPath())
            .arg(tmpFilename);

    QFileInfo tmpFileInfo(tmpPath);
    // TODO: Make check compatible with symbolic links
    if (!tmpFileInfo.absoluteDir().exists())
    {
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

    auto startedOk = doSaveTo(tmpFileInfo.absoluteFilePath().toStdString(), [=](bool success) {
        if (success) {
            QUrl tmpFileUrl = QUrl::fromLocalFile(tmpFileInfo.absoluteFilePath());
            QDesktopServices::openUrl(tmpFileUrl);
        }
    });
    return startedOk;
}

void DraftAttachmentModel::pruneDoneTasks()
{
    if (saveToTask_ && saveToTask_->isDone())
    {
        saveToTask_ = nullptr;
    }
}

bool DraftAttachmentModel::doSaveTo(const std::string &path, const std::function<void(bool /* success */)> &callback)
{
    pruneDoneTasks();
    if (saveToTask_) return false; // only one save to action is allowed at the same time

    saveToListener_ = std::make_shared<ApiMirror::DraftAttachmentsSaveToListener>();
    connect(saveToListener_.get(), &ApiMirror::DraftAttachmentsSaveToListener::_error,
            [=](int64_t convId, int64_t attId, const std::string &path, Kullo::Api::LocalError error)
    {
        K_UNUSED(convId);
        K_UNUSED(attId);
        K_UNUSED(path);
        Log.e() << "File could not be saved to " << path << ", error " << error;
        Applications::KulloApplication::runOnMainThread([=]() {
            callback(false);
        });
    });
    connect(saveToListener_.get(), &ApiMirror::DraftAttachmentsSaveToListener::_finished,
            [=](int64_t convId, int64_t attId, const std::string &path)
    {
        K_UNUSED(convId);
        K_UNUSED(attId);
        K_UNUSED(path);
        Applications::KulloApplication::runOnMainThread([=]() {
            callback(true);
        });
    });

    saveToTask_ = session_->draftAttachments()->saveToAsync(convId_, attId_, path, saveToListener_);

    return true;
}

}
}
