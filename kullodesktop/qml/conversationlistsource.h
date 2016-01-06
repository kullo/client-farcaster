/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <memory>
#include <set>
#include <QAbstractListModel>
#include <kulloclient/util/kulloaddress.h>

#include "kullodesktop/farcaster-forwards.h"

namespace KulloDesktop {
namespace Qml {

class ConversationListSource : public QAbstractListModel
{
    Q_OBJECT

public:
    enum ConversationRoles {
        IdRole = Qt::UserRole, // 256
        ParticipantsAddressesRole,
        ParticipantsListRole,
        ParticipantsRole,
        CountRole,
        CountUndoneRole,
        CountUnreadRole,
        LatestMessageTimestampRole,
        DraftEmptyRole,
        AvatarSrcRole
    };

    explicit ConversationListSource(ClientModel *clientModel, QObject *parent = 0);
    ~ConversationListSource();
    QHash<int, QByteArray> roleNames() const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    quint32 openWhenCreated();
    void setOpenWhenCreated(std::set<Kullo::Util::KulloAddress> participants);

    quint32 unreadMessagesCount();

    ConversationModel *get(quint32 conversationId) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int getIndexForId(quint32 conversationId) const;

    /// Login status changed: Throw away everything.
    void refresh();

signals:
    void conversationsChanged();
    void unreadMessagesCountChanged();

private slots:
    void onConversationAdded(quint32 conversationId);
    void onConversationRemoved(quint32 conversationId);
    void onConversationModified(quint32 conversationId);
    void onParticipantChanged(quint32 conversationId);

private:
    void refreshConversations();
    void connectConversationModelToList(const std::unique_ptr<ConversationModel> &cm);

    const ClientModel *clientModel_;
    std::vector<std::unique_ptr<ConversationModel>> conversationModels_;
    std::set<Kullo::Util::KulloAddress> openWhenCreated_;
};

}
}
