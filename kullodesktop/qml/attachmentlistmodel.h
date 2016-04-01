/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <memory>
#include <QAbstractListModel>
#include <desktoputil/dice/dice-forwards.h>

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
        NoteRole,
        MimeTypeRole
    };

    explicit AttachmentListModel(QObject *parent = 0);
    explicit AttachmentListModel(std::shared_ptr<Kullo::Model::Message> msg, QObject *parent);
    ~AttachmentListModel();
    QHash<int, QByteArray> roleNames() const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    Q_INVOKABLE KulloDesktop::Qml::AttachmentModel* get(quint32 position) const;
    Q_INVOKABLE int count() const;
    Q_INVOKABLE bool saveAllTo(const QUrl &directory) const;

private:
    void refreshAttachmentModels();

    std::shared_ptr<Kullo::Model::Message> msg_;
    std::vector<std::unique_ptr<AttachmentModel>> attachmentModels_;
};

}
}
