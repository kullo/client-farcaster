/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#include "attachmentmodel.h"

#include <QDesktopServices>
#include <QDir>
#include <QUrl>

#include <desktoputil/dice/model/attachment.h>
#include <desktoputil/filesystem.h>
#include <desktoputil/qtypestreamers.h>
#include <desktoputil/stlqt.h>
#include <kulloclient/util/assert.h>
#include <kulloclient/util/exceptions.h>
#include <kulloclient/util/librarylogger.h>

namespace KulloDesktop {
namespace Qml {

AttachmentModel::AttachmentModel(QObject *parent)
    : QObject(parent)
{
    Log.e() << "Don't instantiate Attachment in QML.";
}

AttachmentModel::AttachmentModel(Kullo::Model::Attachment *att, QObject *parent)
    : QObject(parent)
    , attachment_(att)
{
    kulloAssert(attachment_);
    kulloAssert(parent == nullptr);
}

QString AttachmentModel::filename() const
{
    return QString::fromStdString(attachment_->filename());
}

QString AttachmentModel::hash() const
{
    return QString::fromStdString(attachment_->hash());
}

QString AttachmentModel::mimeType() const
{
    return QString::fromStdString(attachment_->mimeType());
}

quint32 AttachmentModel::size() const
{
    return attachment_->size();
}

QString AttachmentModel::note() const
{
    return QString::fromStdString(attachment_->note());
}

bool AttachmentModel::saveTo(const QUrl &url) const
{
    if (!url.isLocalFile())
    {
        Log.e() << "Destination URL " << url << " is not a local file.";
        return false;
    }

    QString localFile = url.toLocalFile();

    try
    {
        attachment_->saveTo(localFile.toStdString());
        return true;
    }
    catch (Kullo::Util::FilesystemError)
    {
        return false;
    }
}

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

    try
    {
        attachment_->saveTo(tmpFileInfo.absoluteFilePath().toStdString());
    }
    catch (Kullo::Util::FilesystemError)
    {
        Log.e() << "File could not be saved to " << tmpFileInfo.absoluteFilePath();
        return false;
    }

    Log.i() << "Opening file " << tmpFileInfo.absoluteFilePath();
    QUrl tmpFileUrl = QUrl::fromLocalFile(tmpFileInfo.absoluteFilePath());
    return QDesktopServices::openUrl(tmpFileUrl);
}

QByteArray AttachmentModel::content() const
{
    return DesktopUtil::StlQt::toQByteArray(attachment_->content());
}

quint32 AttachmentModel::messageId() const
{
    return attachment_->messageId();
}

}
}
