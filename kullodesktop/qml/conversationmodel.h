/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <QDateTime>
#include <QObject>
#include <kulloclient/kulloclient-forwards.h>

#include <desktoputil/dice/model/conversation.h>

#include "kullodesktop/farcaster-forwards.h"
#include "kullodesktop/qml/draftmodel.h"
#include "kullodesktop/qml/messagelistmodel.h"

namespace KulloDesktop {
namespace Qml {

class ConversationModel : public QObject
{
    Q_OBJECT

public:
    explicit ConversationModel(QObject *parent = 0);
    explicit ConversationModel(std::shared_ptr<Kullo::Model::Conversation> conv, QObject *parent);

    Q_PROPERTY(quint32 id READ id NOTIFY idChanged)
    quint32 id() const;

    Q_PROPERTY(QStringList participantsAddresses READ participantsAddresses NOTIFY participantsAddressesChanged)
    QStringList participantsAddresses() const;

    Q_PROPERTY(QString participantsList READ participantsList NOTIFY participantsListChanged)
    QString participantsList() const;

    Q_PROPERTY(QVariantMap participants READ participants NOTIFY participantsChanged)
    QVariantMap participants() const;

    Q_PROPERTY(quint32 count READ count NOTIFY countChanged)
    quint32 count() const;

    Q_PROPERTY(quint32 countUnread READ countUnread NOTIFY countUnreadChanged)
    quint32 countUnread() const;

    Q_PROPERTY(quint32 countUndone READ countUndone NOTIFY countUndoneChanged)
    quint32 countUndone() const;

    Q_PROPERTY(QDateTime latestMessageTimestamp READ latestMessageTimestamp NOTIFY latestMessageTimestampChanged)
    QDateTime latestMessageTimestamp() const;

    Q_PROPERTY(KulloDesktop::Qml::MessageListModel *messages READ messages NOTIFY messagesChanged)
    MessageListModel *messages();

    Q_PROPERTY(KulloDesktop::Qml::DraftModel *draft READ draft NOTIFY draftChanged)
    DraftModel *draft();

    Q_PROPERTY(bool draftEmpty READ draftEmpty NOTIFY draftEmptyChanged)
    bool draftEmpty();

    Q_INVOKABLE void markAllMessagesAsRead();
    Q_INVOKABLE void markAllMessagesAsDone();

    // Non-QML methods

    std::vector<std::unique_ptr<ParticipantModel>> participantsModels() const;

signals:
    void participantChanged(quint32 conversationId);
    void messageAdded(quint32 conversationId, quint32 messageId);
    void messageDeleted(quint32 conversationId, quint32 messageId);
    void latestMessageTimestampChanged();

    // These signals are emitted at once in onConversationModified()
    // to properly update QML properties like in
    // conversationList.model.get(id).count
    void conversationModified(quint32 conversationId);
    void countChanged();
    void countUnreadChanged();
    void countUndoneChanged();
    void draftEmptyChanged();

    // unused signals
    void idChanged();                    // constant
    void participantsChanged();          // constant
    void participantsListChanged();      // constant
    void participantsAddressesChanged(); // constant
    void draftChanged();                 // constant
    void messagesChanged();              // constant

private slots:
    void onConversationModified();
    void onParticipantChanged();
    void onMessageAdded(quint32 messageId);
    void onMessageDeleted(quint32 messageId);

private:
    std::shared_ptr<Kullo::Model::Conversation> conv_;
    std::unique_ptr<MessageListModel> messages_;
    DraftModel draft_;
};

}
}
