/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#include "draftmodel.h"

#include <QMimeDatabase>
#include <QQmlEngine>
#include <QUrl>

#include <apimirror/DraftAttachmentsAddListener.h>
#include <desktoputil/qtypestreamers.h>
#include <kulloclient/api/AsyncTask.h>
#include <kulloclient/api/DraftAttachments.h>
#include <kulloclient/api/Drafts.h>
#include <kulloclient/api/DraftState.h>
#include <kulloclient/util/assert.h>
#include <kulloclient/util/librarylogger.h>
#include <kulloclient/util/misc.h>

#include "kullodesktop/qml/draftattachmentlistmodel.h"

namespace KulloDesktop {
namespace Qml {

DraftModel::DraftModel(QObject *parent)
    : QObject(parent)
{
    Log.f() << "Don't instantiate Draft in QML.";
}

DraftModel::DraftModel(
        const std::shared_ptr<Kullo::Api::Session> &session,
        ApiMirror::EventDispatcher &eventDispatcher,
        Kullo::id_type convId,
        QObject *parent)
    : QObject(parent)
    , session_(session)
    , convId_(convId)
    , attachments_(new DraftAttachmentListModel(session_, eventDispatcher, convId_))
    , draftEmptyCache_(empty())
{
    kulloAssert(session_);

    connect(&eventDispatcher, &ApiMirror::EventDispatcher::draftTextChanged,
            this, [this](Kullo::id_type conversationId)
    {
        if (conversationId == convId_) emit textChanged();
    });
    connect(&eventDispatcher, &ApiMirror::EventDispatcher::draftStateChanged,
            this, [this](Kullo::id_type conversationId)
    {
        if (conversationId == convId_) emit stateChanged();
    });
    connect(&eventDispatcher, &ApiMirror::EventDispatcher::draftAttachmentAdded,
            this, [this](Kullo::id_type conversationId, Kullo::id_type attachmentId)
    {
        K_UNUSED(attachmentId);
        if (conversationId == convId_) onDraftEmptyPotentiallyChanged();
    });
    connect(&eventDispatcher, &ApiMirror::EventDispatcher::draftAttachmentRemoved,
            this, [this](Kullo::id_type conversationId, Kullo::id_type attachmentId)
    {
        K_UNUSED(attachmentId);
        if (conversationId == convId_) onDraftEmptyPotentiallyChanged();
    });

    connect(this, &DraftModel::textChanged, &DraftModel::onDraftEmptyPotentiallyChanged);
}

DraftModel::~DraftModel()
{
    // wait for attachment adder to finish
    addingAttachmentsInProgress_.lock();
    addingAttachmentsInProgress_.unlock();
    save();
}

Kullo::id_type DraftModel::id() const
{
    return convId_;
}

QString DraftModel::state() const
{
    switch (session_->drafts()->state(convId_))
    {
    case Kullo::Api::DraftState::Editing:
        return "editing";
    case Kullo::Api::DraftState::Sending:
        return "sending";
    }
    throw std::logic_error("Invalid state");
}

QString DraftModel::text() const
{
    return QString::fromStdString(session_->drafts()->text(convId_));
}

void DraftModel::setText(QString text)
{
    session_->drafts()->setText(convId_, text.toStdString());
}

DraftAttachmentListModel *DraftModel::attachments()
{
    DraftAttachmentListModel *out = attachments_.get();
    QQmlEngine::setObjectOwnership(out, QQmlEngine::CppOwnership);
    return out;
}

bool DraftModel::empty() const
{
    bool textEmpty = text().trimmed().isEmpty();
    bool attachmentsEmpty = attachments_->count() == 0;
    return textEmpty && attachmentsEmpty;
}

void DraftModel::addAttachment(const QUrl &url)
{
    if (!url.isLocalFile())
    {
        Log.w() << "Attachment URL " << url << " is not a local file. Skipping.";
        return;
    }

    QString localFile = url.toLocalFile();
    QString mimeType = QMimeDatabase().mimeTypeForFile(localFile).name();
    Log.i() << "Adding " << localFile << " of type " << mimeType;

    //TODO use the asynchronity to kill AttachmentsAdderModel and call addAsync from the UI thread
    auto listener = std::make_shared<ApiMirror::DraftAttachmentsAddListener>();
    session_->draftAttachments()->addAsync(
                convId_,
                localFile.toStdString(),
                mimeType.toStdString(),
                listener)->waitUntilDone();
    //TODO handle DraftAttachmentsAddListener::error
}

void DraftModel::removeAttachment(Kullo::id_type index)
{
    session_->draftAttachments()->remove(convId_, index);
}

void DraftModel::save()
{
    //TODO implement cache for text changes that are committed when save() is called
}

void DraftModel::prepareToSend()
{
    QString trimmedText = text().trimmed();
    if (trimmedText != text())
    {
        setText(trimmedText);
        save();
    }
    session_->drafts()->prepareToSend(convId_);
}

std::mutex &DraftModel::addingAttachmentsInProgress()
{
    return addingAttachmentsInProgress_;
}

void DraftModel::onDraftEmptyPotentiallyChanged()
{
    bool draftEmptyCurrent = empty();
    if (draftEmptyCache_ != draftEmptyCurrent)
    {
        draftEmptyCache_ = draftEmptyCurrent;
        emit emptyChanged();
    }
}

}
}
