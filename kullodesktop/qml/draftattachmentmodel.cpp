/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#include "draftattachmentmodel.h"

#include <QDesktopServices>
#include <QDir>
#include <QFileInfo>
#include <QUrl>
#include <desktoputil/dice/model/draftattachment.h>
#include <desktoputil/filesystem.h>
#include <desktoputil/stlqt.h>
#include <desktoputil/qtypestreamers.h>
#include <kulloclient/util/assert.h>
#include <kulloclient/util/exceptions.h>
#include <kulloclient/util/librarylogger.h>

namespace KulloDesktop {
namespace Qml {

DraftAttachmentModel::DraftAttachmentModel(QObject *parent)
    : QObject(parent)
{
    Log.e() << "Don't instantiate DraftAttachment in QML.";
}

DraftAttachmentModel::DraftAttachmentModel(Kullo::Model::DraftAttachment *att, QObject *parent)
    : QObject(parent)
    , draftAttachment_(att)
{
    kulloAssert(draftAttachment_);
    kulloAssert(parent == nullptr);

    connect(draftAttachment_, &Kullo::Model::DraftAttachment::changed,         this, &DraftAttachmentModel::changed);
    connect(draftAttachment_, &Kullo::Model::DraftAttachment::filenameChanged, this, &DraftAttachmentModel::filenameChanged);
    connect(draftAttachment_, &Kullo::Model::DraftAttachment::mimeTypeChanged, this, &DraftAttachmentModel::mimeTypeChanged);
    connect(draftAttachment_, &Kullo::Model::DraftAttachment::noteChanged,     this, &DraftAttachmentModel::noteChanged);
}

QString DraftAttachmentModel::filename() const
{
    return QString::fromStdString(draftAttachment_->filename());
}

quint32 DraftAttachmentModel::index() const
{
    return draftAttachment_->index();
}

QString DraftAttachmentModel::mimeType() const
{
    return QString::fromStdString(draftAttachment_->mimeType());
}

QString DraftAttachmentModel::hash() const
{
    return QString::fromStdString(draftAttachment_->hash());
}

quint32 DraftAttachmentModel::size() const
{
    return draftAttachment_->size();
}

QString DraftAttachmentModel::note() const
{
    return QString::fromStdString(draftAttachment_->note());
}

void DraftAttachmentModel::setNote(QString note)
{
    draftAttachment_->setNote(note.toStdString());
}

void DraftAttachmentModel::save()
{
    draftAttachment_->save();
}

void DraftAttachmentModel::deletePermanently()
{
    draftAttachment_->deletePermanently();
}

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

    try
    {
        draftAttachment_->saveTo(tmpFileInfo.absoluteFilePath().toStdString());
    }
    catch (Kullo::Util::FilesystemError)
    {
        Log.e() << "File could not be saved to: " << tmpFileInfo.absoluteFilePath();
        return false;
    }

    QUrl tmpFileUrl = QUrl::fromLocalFile(tmpFileInfo.absoluteFilePath());
    return QDesktopServices::openUrl(tmpFileUrl);
}

QByteArray DraftAttachmentModel::content() const
{
    return DesktopUtil::StlQt::toQByteArray(draftAttachment_->content());
}

}
}
