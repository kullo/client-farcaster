/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
#pragma once

#include <memory>
#include <QObject>

#include <kulloclient/types.h>
#include <kulloclient/api/Event.h>
#include <kulloclient/api/InternalEvent.h>
#include <kulloclient/api/Session.h>

namespace ApiMirror {

/**
 * @brief Forwards events to Session::notify() and emits signals for Api::Events
 *
 * Connect InternalEvents to onInternalEvent() to have them processed.
 *
 * Instances of this class must be running on the UI thread so that
 * Session::notify() is called from the right thread. If there's no session,
 * nothing is done.
 */
class EventDispatcher : public QObject
{
    Q_OBJECT
public:
    explicit EventDispatcher(QObject *parent = nullptr);

    void setSession(std::weak_ptr<Kullo::Api::Session> &&session);

signals:
    void conversationAdded(Kullo::id_type conversationId);
    void conversationRemoved(Kullo::id_type conversationId);
    void conversationChanged(Kullo::id_type conversationId);
    void conversationLatestMessageTimestampChanged(Kullo::id_type conversationId);
    void draftStateChanged(Kullo::id_type conversationId);
    void draftTextChanged(Kullo::id_type conversationId);
    void draftAttachmentAdded(Kullo::id_type conversationId, Kullo::id_type attachmentId);
    void draftAttachmentRemoved(Kullo::id_type conversationId, Kullo::id_type attachmentId);
    void messageAdded(Kullo::id_type conversationId, Kullo::id_type messageId);
    void messageDeliveryChanged(Kullo::id_type conversationId, Kullo::id_type messageId);
    void messageStateChanged(Kullo::id_type conversationId, Kullo::id_type messageId);
    void messageAttachmentsDownloadedChanged(Kullo::id_type conversationId, Kullo::id_type messageId);
    void messageRemoved(Kullo::id_type conversationId, Kullo::id_type messageId);
    void userSettingsChanged();

public slots:
    void onInternalEvent(const std::shared_ptr<Kullo::Api::InternalEvent> &internalEvent);

private:
    std::weak_ptr<Kullo::Api::Session> session_;
};

}
