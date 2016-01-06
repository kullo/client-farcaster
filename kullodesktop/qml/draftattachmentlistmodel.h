/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <memory>
#include <QAbstractListModel>
#include <desktoputil/dice/dice-forwards.h>

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
        NoteRole,
        MimeTypeRole,
        HashRole
    };

    explicit DraftAttachmentListModel(QObject *parent = 0);
    explicit DraftAttachmentListModel(std::shared_ptr<Kullo::Model::Draft> draft, QObject *parent);
    ~DraftAttachmentListModel();
    QHash<int, QByteArray> roleNames() const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    void reloadModel();

    Q_INVOKABLE KulloDesktop::Qml::DraftAttachmentModel* get(quint32 index) const;
    Q_INVOKABLE int count() const;

private:
    int getPositionForIndex(size_t index) const;
    void loadAttachmentModels();

    std::shared_ptr<Kullo::Model::Draft> draft_;
    std::vector<std::unique_ptr<DraftAttachmentModel>> attachmentModels_;

private slots:
    void onAttachmentsChanged();
};

}
}
