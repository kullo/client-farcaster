/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#include "draftattachmentlistmodel.h"

#include <kulloclient/util/assert.h>
#include <kulloclient/util/librarylogger.h>
#include <QAbstractItemModel>
#include <QQmlEngine>

#include <desktoputil/dice/model/draft.h>

#include "kullodesktop/qml/draftattachmentmodel.h"

namespace KulloDesktop {
namespace Qml {

DraftAttachmentListModel::DraftAttachmentListModel(QObject *parent)
    : QAbstractListModel(parent)
{
    Log.e() << "Don't instantiate DraftAttachmentList in QML.";
}

DraftAttachmentListModel::DraftAttachmentListModel(std::shared_ptr<Kullo::Model::Draft> draft, QObject *parent)
    : QAbstractListModel(parent)
    , draft_(draft)
{
    kulloAssert(draft_);
    loadAttachmentModels();

    connect(draft_.get(), &Kullo::Model::Draft::attachmentsChanged,
            this, &DraftAttachmentListModel::onAttachmentsChanged);
}

DraftAttachmentListModel::~DraftAttachmentListModel()
{
}

QHash<int, QByteArray> DraftAttachmentListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[AttachmentIndexRole] = QByteArrayLiteral("attachmentIndex_");
    roles[FilenameRole]        = QByteArrayLiteral("filename_");
    roles[SizeRole]            = QByteArrayLiteral("size_");
    roles[NoteRole]            = QByteArrayLiteral("note_");
    roles[MimeTypeRole]        = QByteArrayLiteral("mimeType_");
    roles[HashRole]            = QByteArrayLiteral("hash_");
    return roles;
}

QVariant DraftAttachmentListModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || static_cast<unsigned int>(index.row()) >= attachmentModels_.size())
    {
        return QVariant();
    }

    switch (role)
    {
    case AttachmentIndexRole:
        return QVariant(attachmentModels_[index.row()]->index());
    case FilenameRole:
        return QVariant(attachmentModels_[index.row()]->filename());
    case SizeRole:
        return QVariant(attachmentModels_[index.row()]->size());
    case NoteRole:
        return QVariant(attachmentModels_[index.row()]->note());
    case MimeTypeRole:
        return QVariant(attachmentModels_[index.row()]->mimeType());
    case HashRole:
        return QVariant(attachmentModels_[index.row()]->hash());
    default:
        Log.e() << "DraftAttachmentListModel: Invalid role requested: " << role;
        return QVariant();
    }
}

bool DraftAttachmentListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    switch (role) {
    case NoteRole:
        attachmentModels_[index.row()]->setNote(value.toString());
        return true;
    default:
        Log.e() << "DraftAttachmentListModel: Couldn't save model property for role: " << role;
        return false;
    }
}

int DraftAttachmentListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return attachmentModels_.size();
}

void DraftAttachmentListModel::loadAttachmentModels()
{
    for (auto att : draft_->attachments())
    {
        attachmentModels_.emplace_back(new DraftAttachmentModel(att.get(), nullptr));
    }
}

void DraftAttachmentListModel::reloadModel()
{
    beginResetModel();

    attachmentModels_.clear();

    loadAttachmentModels();
    endResetModel();
}

DraftAttachmentModel *DraftAttachmentListModel::get(quint32 index) const
{
    int pos = getPositionForIndex(index);
    if (pos == -1)
    {
        Log.e() << "DraftAttachment not found in DraftAttachmentList. Index: " << index;
        return nullptr;
    }
    else
    {
        DraftAttachmentModel* out = attachmentModels_.at(pos).get();
        QQmlEngine::setObjectOwnership(out, QQmlEngine::CppOwnership);
        return out;
    }
}

int DraftAttachmentListModel::getPositionForIndex(size_t index) const
{
    for (size_t i = 0; i < attachmentModels_.size(); ++i)
    {
        if (attachmentModels_[i]->index() == index)
        {
            return i;
        }
    }
    return -1;
}

int DraftAttachmentListModel::count() const
{
    return rowCount();
}

void DraftAttachmentListModel::onAttachmentsChanged()
{
    Log.i() << "Attachments have changed. Reset model ...";
    reloadModel();
}

}
}
