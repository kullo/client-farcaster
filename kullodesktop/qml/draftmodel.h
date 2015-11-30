/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#pragma once

#include <memory>
#include <mutex>
#include <QObject>
#include <desktoputil/dice/dice-forwards.h>

#include "kullodesktop/farcaster-forwards.h"

namespace KulloDesktop {
namespace Qml {

class DraftModel : public QObject
{
    Q_OBJECT

public:
    explicit DraftModel(QObject *parent = 0);
    explicit DraftModel(std::shared_ptr<Kullo::Model::Draft> draft, QObject *parent);
    ~DraftModel() override;

    Q_PROPERTY(quint32 id READ id NOTIFY idChanged)
    quint32 id() const;

    Q_PROPERTY(quint64 lastModified READ lastModified NOTIFY lastModifiedChanged)
    quint64 lastModified() const;

    Q_PROPERTY(QString state READ state NOTIFY stateChanged)
    QString state() const;

    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    QString text() const;
    void setText(QString text);

    Q_PROPERTY(QString footer READ footer WRITE setFooter NOTIFY footerChanged)
    QString footer() const;
    void setFooter(QString footer);

    Q_PROPERTY(QString senderName READ senderName WRITE setSenderName NOTIFY senderNameChanged)
    QString senderName() const;
    void setSenderName(QString name);

    Q_PROPERTY(QString senderOrganization READ senderOrganization WRITE setSenderOrganization NOTIFY senderOrganizationChanged)
    QString senderOrganization() const;
    void setSenderOrganization(QString organization);

    Q_PROPERTY(KulloDesktop::Qml::DraftAttachmentListModel *attachments READ attachments NOTIFY attachmentsChanged)
    DraftAttachmentListModel *attachments();

    Q_PROPERTY(bool empty READ empty NOTIFY emptyChanged)
    bool empty() const;

    Q_INVOKABLE void addAttachment(const QUrl &url);
    Q_INVOKABLE void removeAttachment(quint32 index);
    Q_INVOKABLE void save();
    /// Do all preparations, e.g. trim text
    Q_INVOKABLE void prepareToSend();

    std::mutex &addingAttachmentsInProgress();

signals:
    void attachmentsChanged();
    void emptyChanged();
    void footerChanged();
    void lastModifiedChanged();
    void senderNameChanged();
    void senderOrganizationChanged();
    void stateChanged();
    void textChanged();
    void textCleared();
    void textPrepared();

    // unused signals
    void idChanged();

private slots:
    void onModified();
    void onTextChanged();
    void onDraftEmptyPotentiallyChanged();

private:
    std::shared_ptr<Kullo::Model::Draft> draft_;
    std::unique_ptr<DraftAttachmentListModel> attachments_;
    bool draftEmptyCache_;
    std::mutex addingAttachmentsInProgress_;
};

}
}
