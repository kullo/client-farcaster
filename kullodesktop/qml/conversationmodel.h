/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <QDateTime>
#include <QObject>

#include <apimirror/eventdispatcher.h>
#include <kulloclient/kulloclient-forwards.h>

#include "kullodesktop/farcaster-forwards.h"
#include "kullodesktop/qml/draftmodel.h"
#include "kullodesktop/qml/messagelistmodel.h"

namespace KulloDesktop {
namespace Qml {

class ConversationModel : public QObject
{
    Q_OBJECT

public:
    explicit ConversationModel(QObject *parent = nullptr);
    ConversationModel(
            const std::shared_ptr<Kullo::Api::Session> &session,
            ApiMirror::EventDispatcher &eventDispatcher,
            Kullo::id_type convId,
            QObject *parent = nullptr);

    Q_PROPERTY(Kullo::id_type id READ id NOTIFY idChanged)
    Kullo::id_type id() const;

    Q_PROPERTY(QStringList participantsAddresses READ participantsAddresses NOTIFY participantsAddressesChanged)
    QStringList participantsAddresses() const;

    Q_PROPERTY(QString participantsList READ participantsList NOTIFY participantsListChanged)
    QString participantsList() const;

    Q_PROPERTY(QVariantMap participants READ participants NOTIFY participantsChanged)
    QVariantMap participants() const;

    Q_PROPERTY(qint32 count READ count NOTIFY countChanged)
    qint32 count() const;

    Q_PROPERTY(qint32 countUnread READ countUnread NOTIFY countUnreadChanged)
    qint32 countUnread() const;

    Q_PROPERTY(qint32 countUndone READ countUndone NOTIFY countUndoneChanged)
    qint32 countUndone() const;

    Q_PROPERTY(qint32 countIncoming READ countIncoming NOTIFY countIncomingChanged)
    qint32 countIncoming() const;

    Q_PROPERTY(qint32 countOutgoing READ countOutgoing NOTIFY countOutgoingChanged)
    qint32 countOutgoing() const;

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

    void notifyChanged();

signals:
    void latestMessageTimestampChanged();

    // These signals are emitted at once in notifyChanged()
    // to properly update QML properties like in
    // conversationList.model.get(id).count
    void countChanged();
    void countUnreadChanged();
    void countUndoneChanged();
    void countIncomingChanged();
    void countOutgoingChanged();
    void draftEmptyChanged();

    // unused signals
    void idChanged();                    // constant
    void participantsChanged();          // constant
    void participantsListChanged();      // constant
    void participantsAddressesChanged(); // constant
    void draftChanged();                 // constant
    void messagesChanged();              // constant

private:
    std::shared_ptr<Kullo::Api::Session> session_;
    ApiMirror::EventDispatcher *eventDispatcher_ = nullptr;
    Kullo::id_type convId_ = -1;
    std::unique_ptr<MessageListModel> messages_;
    DraftModel draft_;
};

}
}
