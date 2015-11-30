/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#pragma once

#include <QDateTime>
#include <QObject>
#include <desktoputil/dice/dice-forwards.h>

#include "kullodesktop/qml/attachmentlistmodel.h"
#include "kullodesktop/qml/participantmodel.h"

namespace KulloDesktop {
namespace Qml {

class MessageModel : public QObject
{
    Q_OBJECT

public:
    explicit MessageModel(QObject *parent = 0);
    explicit MessageModel(std::shared_ptr<Kullo::Model::Message> msg, QObject *parent);

    Q_PROPERTY(quint32 id READ id NOTIFY idChanged)
    quint32 id() const;

    Q_PROPERTY(quint32 conversationId READ conversationId NOTIFY conversationIdChanged)
    quint32 conversationId() const;

    Q_PROPERTY(ParticipantModel *sender READ sender NOTIFY senderChanged)
    ParticipantModel *sender();

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

    Q_PROPERTY(bool deleted READ deleted NOTIFY deletedChanged)
    bool deleted() const;

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

    void markAsReadAndDone();

signals:
    // Add message ID parameter for MessageListModel
    void attachmentsDownloaded(quint32);
    // Forward signal from Model::Message to MessageListModel
    void stateChanged(quint32);
    void deliveryStatusChanged(quint32);

    void idChanged();
    void conversationIdChanged();
    void senderChanged();
    void readChanged();
    void doneChanged();
    void dateSentChanged();
    void dateReceivedChanged();
    void deletedChanged();
    void textChanged();
    void footerChanged();
    void attachmentsChanged();
    void attachmentsReadyChanged();

private slots:
    void onAttachmentsDownloaded();

private:
    std::shared_ptr<Kullo::Model::Message> msg_;
    ParticipantModel sender_;
    AttachmentListModel attachments_;
    QVariant attachmentsDownloaded_;
};

}
}
