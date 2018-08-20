/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
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

void DraftModel::addAttachment(const QString &localFile)
{
    QString mimeType = QMimeDatabase().mimeTypeForFile(localFile).name();
    Log.i() << "Adding " << localFile << " of type " << mimeType;

    auto listener = Kullo::nn_make_shared<ApiMirror::DraftAttachmentsAddListener>();
    connect(listener.get(), &ApiMirror::DraftAttachmentsAddListener::_progressed,
            this, [&] (Kullo::id_type convId, Kullo::id_type attId, int64_t bytesProcessed, int64_t bytesTotal) {
        K_UNUSED(convId);
        K_UNUSED(attId);
        emit addingAttachmentProgressed(bytesProcessed, bytesTotal);
    });
    connect(listener.get(), &ApiMirror::DraftAttachmentsAddListener::_finished,
            this, &DraftModel::addingAttachmentFinished);
    connect(listener.get(), &ApiMirror::DraftAttachmentsAddListener::_error,
            this, [&] (Kullo::id_type convId, const std::string &path, Kullo::Api::LocalError error) {
        K_UNUSED(convId);
        QString filename = QFileInfo(QString::fromStdString(path)).fileName();
        emit addingAttachmentError(ApiMirror::Enums::LocalErrorHolder::convert(error), filename);
    });
    addAttachmentTask_ = session_->draftAttachments()->addAsync(
                convId_,
                localFile.toStdString(),
                mimeType.toStdString(),
                listener).as_nullable();
}

std::mutex &DraftModel::addingAttachmentsInProgress()
{
    return addingAttachmentsInProgress_;
}

void DraftModel::onDraftEmptyPotentiallyChanged()
{
    bool draftEmptyCurrent = empty();
    if (!draftEmptyCache_ || *draftEmptyCache_ != draftEmptyCurrent)
    {
        draftEmptyCache_ = draftEmptyCurrent;
        emit emptyChanged();
    }
}

}
}
