/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#include "attachmentlistmodel.h"

#include <QQmlEngine>

#include <kulloclient/api/MessageAttachments.h>
#include <kulloclient/util/assert.h>
#include <kulloclient/util/librarylogger.h>
#include <desktoputil/filesystem.h>
#include <desktoputil/qtypestreamers.h>

#include "kullodesktop/qml/attachmentmodel.h"

namespace KulloDesktop {
namespace Qml {

AttachmentListModel::AttachmentListModel(QObject *parent)
    : QAbstractListModel(parent)
{
    Log.f() << "Don't instantiate AttachmentList in QML.";
}

AttachmentListModel::AttachmentListModel(
        const std::shared_ptr<Kullo::Api::Session> &session,
        Kullo::id_type msgId,
        QObject *parent)
    : QAbstractListModel(parent)
    , session_(session)
    , msgId_(msgId)
{
    refreshAttachmentModels();
}

AttachmentListModel::~AttachmentListModel()
{
}

QHash<int, QByteArray> AttachmentListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[FilenameRole] = QByteArrayLiteral("filename_");
    roles[HashRole]     = QByteArrayLiteral("hash_");
    roles[SizeRole]     = QByteArrayLiteral("size_");
    roles[MimeTypeRole] = QByteArrayLiteral("mimeType_");
    return roles;
}

QVariant AttachmentListModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || static_cast<unsigned int>(index.row()) >= attachmentModels_.size())
    {
        return QVariant();
    }

    switch (role)
    {
    case FilenameRole:
        return QVariant::fromValue(attachmentModels_[index.row()]->filename());
    case HashRole:
        return QVariant::fromValue(attachmentModels_[index.row()]->hash());
    case SizeRole:
        return QVariant::fromValue(attachmentModels_[index.row()]->size());
    case MimeTypeRole:
        return QVariant::fromValue(attachmentModels_[index.row()]->mimeType());

    default:
        return QVariant();
    }
}

int AttachmentListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return attachmentModels_.size();
}

AttachmentModel *AttachmentListModel::get(quint32 position) const
{
    AttachmentModel* out = attachmentModels_.at(position).get();
    QQmlEngine::setObjectOwnership(out, QQmlEngine::CppOwnership);
    return out;
}

int AttachmentListModel::count() const
{
    return attachmentModels_.size();
}

bool AttachmentListModel::saveAllTo(const QUrl &directory) const
{
    kulloAssert(directory.isLocalFile());
    auto localDirectory = directory.toLocalFile();

    for (auto &attachment : attachmentModels_)
    {
        auto target = localDirectory + "/" + attachment->filename();
        while (QFileInfo(target).exists())
        {
            target = DesktopUtil::Filesystem::increaseFilenameCounter(QUrl::fromLocalFile(target)).toLocalFile();
        }
        Log.d() << "Save to " << target;
        if (!attachment->saveTo(QUrl::fromLocalFile(target))) return false;
    }

    return true;
}

void AttachmentListModel::refreshAttachmentModels()
{
    attachmentModels_.clear();

    for (auto attId : session_->messageAttachments()->allForMessage(msgId_))
    {
        attachmentModels_.emplace_back(new AttachmentModel(session_, msgId_, attId));
    }
}

}
}
