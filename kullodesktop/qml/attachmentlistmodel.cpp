/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#include "attachmentlistmodel.h"

#include <QQmlEngine>
#include <kulloclient/util/librarylogger.h>

#include <desktoputil/dice/model/message.h>

#include "kullodesktop/qml/attachmentmodel.h"

namespace KulloDesktop {
namespace Qml {

AttachmentListModel::AttachmentListModel(QObject *parent)
    : QAbstractListModel(parent)
{
    Log.e() << "Don't instantiate AttachmentList in QML.";
}

AttachmentListModel::AttachmentListModel(std::shared_ptr<Kullo::Model::Message> msg, QObject *parent)
    : QAbstractListModel(parent)
    , msg_(msg)
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
    roles[SizeRole]     = QByteArrayLiteral("size_");
    roles[NoteRole]     = QByteArrayLiteral("note_");
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
    case SizeRole:
        return QVariant::fromValue(attachmentModels_[index.row()]->size());
    case NoteRole:
        return QVariant::fromValue(attachmentModels_[index.row()]->note());
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

void AttachmentListModel::refreshAttachmentModels()
{
    attachmentModels_.clear();

    for (auto att : msg_->attachments())
    {
        attachmentModels_.emplace_back(new AttachmentModel(att.get(), nullptr));
    }
}

}
}
