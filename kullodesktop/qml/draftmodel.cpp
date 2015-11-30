/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#include "draftmodel.h"

#include <desktoputil/dice/model/draft.h>
#include <desktoputil/qtypestreamers.h>
#include <kulloclient/util/assert.h>
#include <kulloclient/util/librarylogger.h>
#include <QMimeDatabase>
#include <QQmlEngine>
#include <QUrl>

#include "kullodesktop/qml/draftattachmentlistmodel.h"

namespace KulloDesktop {
namespace Qml {

DraftModel::DraftModel(QObject *parent)
    : QObject(parent)
{
    Log.e() << "Don't instantiate Draft in QML.";
}

DraftModel::DraftModel(std::shared_ptr<Kullo::Model::Draft> draft, QObject *parent)
    : QObject(parent)
    , draft_(draft)
    , attachments_(new DraftAttachmentListModel(draft, nullptr))
    , draftEmptyCache_(empty())
{
    kulloAssert(draft_);
    kulloAssert(parent == nullptr);

    connect(draft_.get(), &Kullo::Model::Draft::modified,           this, &DraftModel::onModified);
    connect(draft_.get(), &Kullo::Model::Draft::modified,           this, &DraftModel::onDraftEmptyPotentiallyChanged);
    connect(draft_.get(), &Kullo::Model::Draft::textChanged,        this, &DraftModel::onTextChanged);
    connect(draft_.get(), &Kullo::Model::Draft::textChanged,        this, &DraftModel::onDraftEmptyPotentiallyChanged);
    connect(draft_.get(), &Kullo::Model::Draft::footerChanged,      this, &DraftModel::footerChanged);
    connect(draft_.get(), &Kullo::Model::Draft::attachmentsChanged, this, &DraftModel::attachmentsChanged);
    connect(draft_.get(), &Kullo::Model::Draft::attachmentsChanged, this, &DraftModel::onDraftEmptyPotentiallyChanged);
    connect(draft_.get(), &Kullo::Model::Draft::stateChanged,       this, &DraftModel::stateChanged);
}

DraftModel::~DraftModel()
{
    // wait for attachment adder to finish
    addingAttachmentsInProgress_.lock();
    addingAttachmentsInProgress_.unlock();
    save();
}

quint32 DraftModel::id() const
{
    return draft_->id();
}

quint64 DraftModel::lastModified() const
{
    return draft_->lastModified();
}

QString DraftModel::state() const
{
    switch (draft_->state())
    {
    case Kullo::Dao::DraftState::Editing:
        return "editing";
    case Kullo::Dao::DraftState::Sending:
        return "sending";
    }
    throw std::logic_error("Invalid state");
}

QString DraftModel::text() const
{
    return QString::fromStdString(draft_->text());
}

void DraftModel::setText(QString text)
{
    Log.d() << "DraftModel::setText(" << text << ")";

    // Workaround for
    // https://bugreports.qt-project.org/browse/QTBUG-42773
    if (draft_->state() == Kullo::Dao::DraftState::Editing)
    {
        Log.d() << "DraftModel::setText() state = EDITING, ok.";
        draft_->setText(text.toStdString());
    }
    else
    {
        Log.d() << "DraftModel::setText() state = SENDING, ignored.";
    }
}

QString DraftModel::footer() const
{
    return QString::fromStdString(draft_->footer());
}

void DraftModel::setFooter(QString footer)
{
    draft_->setFooter(footer.toStdString());
}

QString DraftModel::senderName() const
{
    return QString::fromStdString(draft_->senderName());
}

void DraftModel::setSenderName(QString name)
{
    draft_->setSenderName(name.toStdString());
}

QString DraftModel::senderOrganization() const
{
    return QString::fromStdString(draft_->senderOrganization());
}

void DraftModel::setSenderOrganization(QString organization)
{
    draft_->setSenderOrganization(organization.toStdString());
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

    draft_->addAttachment(localFile.toStdString(), mimeType.toStdString());
}

void DraftModel::removeAttachment(quint32 index)
{
    draft_->removeAttachment(index);
}

void DraftModel::save()
{
    draft_->save();
}

void DraftModel::prepareToSend()
{
    Log.d() << "DraftModel::prepareToSend()";
    QString trimmedText = text().trimmed();
    if (trimmedText != text())
    {
        setText(trimmedText);
        emit textPrepared();
    }

    draft_->prepareToSend();
    Log.d() << "End DraftModel::prepareToSend()";
}

std::mutex &DraftModel::addingAttachmentsInProgress()
{
    return addingAttachmentsInProgress_;
}

void DraftModel::onModified()
{
    Log.d() << "Model::Draft::modified(" << draft_->text() << ")";

    // Model::Draft.modified is emitted from messages sender in bg thread only
    //
    // 1. Clear all
    // 2. Set state to "editing" to unlock QML

    attachments_->reloadModel();
    emit footerChanged();
    emit lastModifiedChanged();
    if (draft_->text().empty()) emit textCleared();
    else                        emit textChanged();
    emit stateChanged();
}

void DraftModel::onTextChanged()
{
    Log.d() << "Model::Draft::textChanged(" << draft_->text() << ")";
    emit textChanged();
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
