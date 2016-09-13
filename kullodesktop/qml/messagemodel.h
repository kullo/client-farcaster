/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <QDateTime>
#include <QObject>

#include <apimirror/eventdispatcher.h>
#include <kulloclient/types.h>
#include <kulloclient/api/Session.h>

#include "kullodesktop/qml/attachmentlistmodel.h"
#include "kullodesktop/qml/sender.h"

namespace KulloDesktop {
namespace Qml {

class MessageModel : public QObject
{
    Q_OBJECT

public:
    explicit MessageModel(QObject *parent = nullptr);
    MessageModel(
            const std::shared_ptr<Kullo::Api::Session> &session,
            ApiMirror::EventDispatcher &eventDispatcher,
            Kullo::id_type msgId,
            QObject *parent = nullptr);

    Q_PROPERTY(Kullo::id_type id READ id NOTIFY idChanged)
    Kullo::id_type id() const;

    Q_PROPERTY(Kullo::id_type conversationId READ conversationId NOTIFY conversationIdChanged)
    Kullo::id_type conversationId() const;

    Q_PROPERTY(Sender *sender READ sender NOTIFY senderChanged)
    Sender *sender();

    Q_PROPERTY(bool read READ read WRITE setRead NOTIFY readChanged)
    bool read() const;
    void setRead(bool value);

    Q_PROPERTY(bool done READ done WRITE setDone NOTIFY doneChanged)
    bool done() const;
    void setDone(bool value);

    Q_PROPERTY(QDateTime dateSent READ dateSent NOTIFY dateSentChanged)
    QDateTime dateSent() const;

    Q_PROPERTY(QDateTime dateReceived READ dateReceived NOTIFY dateReceivedChanged)
    QDateTime dateReceived() const;

    Q_PROPERTY(QString text READ text NOTIFY textChanged)
    QString text() const;

    // used in MessageListSource
    QString textAsHtml() const;

    Q_PROPERTY(QString footer READ footer NOTIFY footerChanged)
    QString footer() const;

    Q_PROPERTY(QString deliveryStatus READ deliveryStatus NOTIFY deliveryStatusChanged)
    QString deliveryStatus() const;

    Q_PROPERTY(AttachmentListModel *attachments READ attachments NOTIFY attachmentsChanged)
    AttachmentListModel *attachments();

    Q_PROPERTY(bool attachmentsReady READ attachmentsReady NOTIFY attachmentsReadyChanged)
    bool attachmentsReady() const;

    Q_INVOKABLE void deletePermanently();

    bool operator<(const MessageModel &other) const;
    bool operator>(const MessageModel &other) const;

signals:
    // Add message ID parameter for MessageListModel
    void attachmentsDownloaded(Kullo::id_type);
    // Forward signal from Model::Message to MessageListModel
    void stateChanged(Kullo::id_type);
    void deliveryStatusChanged(Kullo::id_type);

    void idChanged();
    void conversationIdChanged();
    void senderChanged();
    void readChanged();
    void doneChanged();
    void dateSentChanged();
    void dateReceivedChanged();
    void textChanged();
    void footerChanged();
    void attachmentsChanged();
    void attachmentsReadyChanged();

private slots:
    void onAttachmentsDownloaded();

private:
    std::shared_ptr<Kullo::Api::Session> session_;
    Kullo::id_type msgId_ = -1;
    Sender sender_;
    AttachmentListModel attachments_;
    QVariant attachmentsDownloaded_;
};

}
}
