/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
#pragma once

#include <memory>
#include <QAbstractListModel>

#include <kulloclient/types.h>
#include <kulloclient/api/Session.h>

#include "kullodesktop/farcaster-forwards.h"

namespace KulloDesktop {
namespace Qml {

class AttachmentListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum AttachmentRoles {
        FilenameRole = Qt::UserRole,
        HashRole,
        SizeRole,
        MimeTypeRole
    };

    explicit AttachmentListModel(QObject *parent = nullptr);
    AttachmentListModel(
            const std::shared_ptr<Kullo::Api::Session> &session,
            Kullo::id_type msgId,
            QObject *parent = nullptr);
    ~AttachmentListModel();
    QHash<int, QByteArray> roleNames() const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    Q_INVOKABLE KulloDesktop::Qml::AttachmentModel* get(quint32 position) const;
    Q_INVOKABLE int count() const;
    Q_INVOKABLE bool saveAllToAsync(const QUrl &directory) const;

private:
    void refreshAttachmentModels();

    std::shared_ptr<Kullo::Api::Session> session_;
    Kullo::id_type msgId_ = -1;
    std::vector<std::unique_ptr<AttachmentModel>> attachmentModels_;
};

}
}
