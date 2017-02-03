/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#include "draftattachmentlistmodel.h"

#include <QAbstractItemModel>
#include <QQmlEngine>

#include <kulloclient/api/DraftAttachments.h>
#include <kulloclient/util/assert.h>
#include <kulloclient/util/librarylogger.h>
#include <kulloclient/util/misc.h>

#include "kullodesktop/qml/draftattachmentmodel.h"

namespace KulloDesktop {
namespace Qml {

DraftAttachmentListModel::DraftAttachmentListModel(QObject *parent)
    : QAbstractListModel(parent)
{
    Log.f() << "Don't instantiate DraftAttachmentList in QML.";
}

DraftAttachmentListModel::DraftAttachmentListModel(
        const std::shared_ptr<Kullo::Api::Session> &session,
        ApiMirror::EventDispatcher &eventDispatcher,
        Kullo::id_type convId,
        QObject *parent)
    : QAbstractListModel(parent)
    , session_(session)
    , convId_(convId)
{
    kulloAssert(session_);
    loadAttachmentModels();

    connect(&eventDispatcher, &ApiMirror::EventDispatcher::draftAttachmentAdded,
            this, &DraftAttachmentListModel::onAttachmentsChanged);
    connect(&eventDispatcher, &ApiMirror::EventDispatcher::draftAttachmentRemoved,
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
    roles[MimeTypeRole]        = QByteArrayLiteral("mimeType_");
    roles[HashRole]            = QByteArrayLiteral("hash_");
    return roles;
}

QVariant DraftAttachmentListModel::data(const QModelIndex &index, int role) const
{
    auto row = static_cast<unsigned int>(index.row());
    if (index.row() < 0 || row >= attachmentModels_.size())
    {
        return QVariant();
    }

    switch (role)
    {
    case AttachmentIndexRole:
        return QVariant::fromValue(attachmentModels_[row]->index());
    case FilenameRole:
        return attachmentModels_[row]->filename();
    case SizeRole:
        return QVariant::fromValue(attachmentModels_[row]->size());
    case MimeTypeRole:
        return attachmentModels_[row]->mimeType();
    case HashRole:
        return attachmentModels_[row]->hash();
    default:
        Log.e() << "DraftAttachmentListModel: Invalid role requested: " << role;
        return QVariant();
    }
}

int DraftAttachmentListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return attachmentModels_.size();
}

void DraftAttachmentListModel::loadAttachmentModels()
{
    for (auto attId : session_->draftAttachments()->allForDraft(convId_))
    {
        attachmentModels_.emplace_back(new DraftAttachmentModel(session_, convId_, attId));
    }
}

void DraftAttachmentListModel::reloadModel()
{
    beginResetModel();

    attachmentModels_.clear();

    loadAttachmentModels();
    endResetModel();
}

DraftAttachmentModel *DraftAttachmentListModel::get(Kullo::id_type index) const
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

int DraftAttachmentListModel::getPositionForIndex(Kullo::id_type index) const
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

void DraftAttachmentListModel::onAttachmentsChanged(Kullo::id_type conversationId, Kullo::id_type attachmentId)
{
    K_UNUSED(attachmentId);

    if (conversationId == convId_) {
        Log.i() << "Attachments have changed. Reset model ...";
        reloadModel();
    }
}

}
}
