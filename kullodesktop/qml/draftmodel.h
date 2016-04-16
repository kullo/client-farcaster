/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <memory>
#include <mutex>
#include <QObject>

#include <apimirror/eventdispatcher.h>
#include <kulloclient/types.h>
#include <kulloclient/api/Session.h>

#include "kullodesktop/farcaster-forwards.h"

namespace KulloDesktop {
namespace Qml {

class DraftModel : public QObject
{
    Q_OBJECT

public:
    explicit DraftModel(QObject *parent = nullptr);
    DraftModel(
            const std::shared_ptr<Kullo::Api::Session> &session,
            ApiMirror::EventDispatcher &eventDispatcher,
            Kullo::id_type convId,
            QObject *parent = nullptr);
    ~DraftModel() override;

    Q_PROPERTY(Kullo::id_type id READ id NOTIFY idChanged)
    Kullo::id_type id() const;

    Q_PROPERTY(QString state READ state NOTIFY stateChanged)
    QString state() const;

    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    QString text() const;
    void setText(QString text);

    Q_PROPERTY(KulloDesktop::Qml::DraftAttachmentListModel *attachments READ attachments NOTIFY attachmentsChanged)
    DraftAttachmentListModel *attachments();

    Q_PROPERTY(bool empty READ empty NOTIFY emptyChanged)
    bool empty() const;

    Q_INVOKABLE void addAttachment(const QUrl &url);
    Q_INVOKABLE void removeAttachment(Kullo::id_type index);
    Q_INVOKABLE void save();
    /// Do all preparations, e.g. trim text
    Q_INVOKABLE void prepareToSend();

    std::mutex &addingAttachmentsInProgress();

signals:
    void emptyChanged();
    void stateChanged();
    void textChanged();

    // unused signals
    void idChanged();
    void attachmentsChanged();

private slots:
    void onDraftEmptyPotentiallyChanged();

private:
    std::shared_ptr<Kullo::Api::Session> session_;
    Kullo::id_type convId_ = -1;
    std::unique_ptr<DraftAttachmentListModel> attachments_;
    bool draftEmptyCache_ = true;
    std::mutex addingAttachmentsInProgress_;
};

}
}
