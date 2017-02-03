/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#pragma once

#include <memory>
#include <QAbstractListModel>

#include <apimirror/eventdispatcher.h>
#include <kulloclient/types.h>
#include <kulloclient/api/Session.h>

#include "kullodesktop/farcaster-forwards.h"

namespace KulloDesktop {
namespace Qml {

class DraftAttachmentListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum DraftAttachmentRoles {
        AttachmentIndexRole = Qt::UserRole,
        FilenameRole,
        SizeRole,
        MimeTypeRole,
        HashRole
    };

    explicit DraftAttachmentListModel(QObject *parent = nullptr);
    DraftAttachmentListModel(
            const std::shared_ptr<Kullo::Api::Session> &session,
            ApiMirror::EventDispatcher &eventDispatcher,
            Kullo::id_type convId,
            QObject *parent = nullptr);
    ~DraftAttachmentListModel();
    QHash<int, QByteArray> roleNames() const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    void reloadModel();

    Q_INVOKABLE KulloDesktop::Qml::DraftAttachmentModel* get(Kullo::id_type index) const;
    Q_INVOKABLE int count() const;

private slots:
    void onAttachmentsChanged(Kullo::id_type conversationId, Kullo::id_type attachmentId);

private:
    int getPositionForIndex(Kullo::id_type index) const;
    void loadAttachmentModels();

    std::shared_ptr<Kullo::Api::Session> session_;
    Kullo::id_type convId_ = -1;
    std::vector<std::unique_ptr<DraftAttachmentModel>> attachmentModels_;
};

}
}
