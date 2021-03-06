/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
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

    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    QString title(int maxChars = -1) const;

    Q_PROPERTY(QStringList participantsAddresses READ participantsAddresses NOTIFY participantsAddressesChanged)
    QStringList participantsAddresses() const;

    Q_PROPERTY(QVariantMap participantNames READ participantNames NOTIFY participantNamesChanged)
    QVariantMap participantNames() const;

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

    // Let central point (conversations list) collect and distribute changed
    // signals in order to avoid that every conversation needs to listen on every
    // conversation changed event and check id
    void notifyChanged();

signals:
    // These signals are emitted at once in notifyChanged()
    // to properly update QML properties like in
    // conversationList.model.get(id).count
    void titleChanged();
    void countChanged();
    void countUnreadChanged();
    void countUndoneChanged();
    void countIncomingChanged();
    void countOutgoingChanged();
    void latestMessageTimestampChanged();
    void draftEmptyChanged();

    // unused signals
    void idChanged();                    // constant
    void participantNamesChanged();      // constant
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
