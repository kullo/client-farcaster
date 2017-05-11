/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#include "apimirror/eventdispatcher.h"

namespace ApiMirror {

EventDispatcher::EventDispatcher(QObject *parent)
    : QObject(parent)
{}

void EventDispatcher::setSession(std::weak_ptr<Kullo::Api::Session> &&session)
{
    session_ = std::move(session);
}

void EventDispatcher::onInternalEvent(const std::shared_ptr<Kullo::Api::InternalEvent> &internalEvent)
{
    using namespace Kullo::Api;

    if (auto session = session_.lock())
    {
        for (const auto &event : session->notify(internalEvent))
        {
            switch (event.event)
            {
            case EventType::ConversationAdded:
                emit conversationAdded(event.conversationId);
                break;
            case EventType::ConversationRemoved:
                emit conversationRemoved(event.conversationId);
                break;
            case EventType::ConversationChanged:
                emit conversationChanged(event.conversationId);
                break;
            case EventType::ConversationLatestMessageTimestampChanged:
                emit conversationLatestMessageTimestampChanged(event.conversationId);
                break;
            case EventType::DraftStateChanged:
                emit draftStateChanged(event.conversationId);
                break;
            case EventType::DraftTextChanged:
                emit draftTextChanged(event.conversationId);
                break;
            case EventType::DraftAttachmentAdded:
                emit draftAttachmentAdded(event.conversationId, event.attachmentId);
                break;
            case EventType::DraftAttachmentRemoved:
                emit draftAttachmentRemoved(event.conversationId, event.attachmentId);
                break;
            case EventType::MessageAdded:
                emit messageAdded(event.conversationId, event.messageId);
                break;
            case EventType::MessageDeliveryChanged:
                emit messageDeliveryChanged(event.conversationId, event.messageId);
                break;
            case EventType::MessageStateChanged:
                emit messageStateChanged(event.conversationId, event.messageId);
                break;
            case EventType::MessageAttachmentsDownloadedChanged:
                emit messageAttachmentsDownloadedChanged(event.conversationId, event.messageId);
                break;
            case EventType::MessageRemoved:
                emit messageRemoved(event.conversationId, event.messageId);
                break;
            case EventType::UserSettingsChanged:
                emit userSettingsChanged();
                break;
            }
        }
    }
}

}
