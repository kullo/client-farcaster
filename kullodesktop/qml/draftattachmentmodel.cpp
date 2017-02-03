/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#include "draftattachmentmodel.h"

#include <boost/optional.hpp>
#include <boost/optional/optional_io.hpp>
#include <QDesktopServices>
#include <QDir>
#include <QFileInfo>
#include <QUrl>

#include <apimirror/DraftAttachmentsContentListener.h>
#include <apimirror/DraftAttachmentsSaveToListener.h>
#include <desktoputil/filesystem.h>
#include <desktoputil/stlqt.h>
#include <desktoputil/qtypestreamers.h>
#include <kulloclient/api/AsyncTask.h>
#include <kulloclient/api/DraftAttachments.h>
#include <kulloclient/api_impl/debug.h>
#include <kulloclient/util/assert.h>
#include <kulloclient/util/librarylogger.h>
#include <kulloclient/util/misc.h>

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

//TODO make DraftAttachmentModel::open asynchronous
bool DraftAttachmentModel::open()
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

    if (!doSaveTo(tmpFileInfo.absoluteFilePath().toStdString())) return false;

    QUrl tmpFileUrl = QUrl::fromLocalFile(tmpFileInfo.absoluteFilePath());
    return QDesktopServices::openUrl(tmpFileUrl);
}

QByteArray DraftAttachmentModel::content() const
{
    auto listener = std::make_shared<ApiMirror::DraftAttachmentsContentListener>();
    std::vector<unsigned char> data;
    connect(listener.get(), &ApiMirror::DraftAttachmentsContentListener::_finished,
            [&data](int64_t convId, int64_t attId, const std::vector<uint8_t> &content)
    {
        K_UNUSED(convId);
        K_UNUSED(attId);
        data = content;
    });

    // Synchronous operation is no problem here, as content() is only used for
    // ImageProviders which operate asynchronously.
    session_->draftAttachments()->contentAsync(convId_, attId_, listener)->waitUntilDone();
    return DesktopUtil::StlQt::toQByteArray(data);
}

bool DraftAttachmentModel::doSaveTo(const std::string &path) const
{
    auto listener = std::make_shared<ApiMirror::DraftAttachmentsSaveToListener>();
    boost::optional<Kullo::Api::LocalError> saveErr;
    connect(listener.get(), &ApiMirror::DraftAttachmentsSaveToListener::_error,
            [&saveErr](int64_t convId, int64_t attId, const std::string &path, Kullo::Api::LocalError error)
    {
        K_UNUSED(convId);
        K_UNUSED(attId);
        K_UNUSED(path);
        saveErr = error;
    });

    session_->draftAttachments()->saveToAsync(convId_, attId_, path, listener)->waitUntilDone();

    if (saveErr)
    {
        Log.e() << "File could not be saved to " << path << ", error " << saveErr;
        return false;
    }
    return true;
}

}
}
