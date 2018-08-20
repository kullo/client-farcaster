/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
#include "attachmentsadder.h"

#include <QQmlEngine>
#include <desktoputil/asynctask.h>
#include <desktoputil/qtypestreamers.h>
#include <kulloclient/util/assert.h>
#include <kulloclient/util/exceptions.h>
#include <kulloclient/util/filesystem.h>
#include <kulloclient/util/librarylogger.h>

#include "kullodesktop/qml/draftmodel.h"

namespace KulloDesktop {
namespace QmlComponents {

AttachmentsAdder::AttachmentsAdder(QObject *parent) :
    QObject(parent),
    cancel_(false)
{
}

AttachmentsAdder::~AttachmentsAdder()
{
    cancel_ = true;
}

Qml::DraftModel *AttachmentsAdder::target() const
{
    QQmlEngine::setObjectOwnership(draftModel_, QQmlEngine::CppOwnership);
    return draftModel_;
}

void AttachmentsAdder::setTarget(Qml::DraftModel *target)
{
    if (draftModel_ != target)
    {
        draftModel_ = target;
        connect(draftModel_, &Qml::DraftModel::addingAttachmentProgressed,
                this, &AttachmentsAdder::onAddingAttachmentProgressed);
        connect(draftModel_, &Qml::DraftModel::addingAttachmentFinished,
                this, &AttachmentsAdder::onAddingAttachmentFinished);
        connect(draftModel_, &Qml::DraftModel::addingAttachmentError,
                this, &AttachmentsAdder::onAddingAttachmentError);
        emit targetChanged();
    }
}

bool AttachmentsAdder::addAttachments(const QList<QUrl> &urls)
{
    if (!working_.try_lock()) return false;

    sizeAllAttachments_ = 0;
    sizeSumReadyAttachments_ = 0;
    attachmentsToBeAdded_.clear();
    for (const auto url : urls)
    {
        if (!url.isLocalFile())
        {
            Log.w() << "Attachment URL " << url << " is not a local file. Skipping.";
            continue;
        }

        AttachmentToBeAdded newAttachment;
        newAttachment.source = url.toLocalFile();
        try
        {
            newAttachment.size = Kullo::Util::Filesystem::fileSize(newAttachment.source.toStdString());
        }
        catch (Kullo::Util::FilesystemError &)
        {
            Log.e() << "Could not get size of " << newAttachment.source << ". Skipping.";
            continue;
        }
        sizeAllAttachments_ += newAttachment.size;
        attachmentsToBeAdded_.push_back(newAttachment);
    }
    cancel_ = false;
    kulloAssert(draftModel_);

    // prevent DraftModel from being deleted as long as we are working
    draftModel_->addingAttachmentsInProgress().lock();

    emit addingAttachmentsStarted();
    auto available = addNextAttachment();
    if (!available)
    {
        doneAddingAttachments();
        emit addingAttachmentsFinished();
    }

    return true;
}

void AttachmentsAdder::onAddingAttachmentProgressed(int64_t bytesProcessed, int64_t bytesTotal)
{
    K_UNUSED(bytesTotal);

    if (sizeAllAttachments_ > 0)
    {
        int percentages = (100*(sizeSumReadyAttachments_+bytesProcessed)) / sizeAllAttachments_;
        if (percentages > 100) percentages = 100;
        Log.d() << "Adding ... " << percentages << "%";
        emit progressed(percentages);
    }
}

void AttachmentsAdder::onAddingAttachmentError(
        ApiMirror::Enums::LocalErrorHolder::LocalError error,
        const QString &filename)
{
    doneAddingAttachments();
    emit addingAttachmentsError(error, filename);
}

void AttachmentsAdder::onAddingAttachmentFinished()
{
    sizeSumReadyAttachments_ += currentlyAddedAttachment_->size;

    if (cancel_)
    {
        doneAddingAttachments();
        emit addingAttachmentsFinished();
    }
    else
    {
        auto available = addNextAttachment();
        if (!available)
        {
            doneAddingAttachments();
            emit addingAttachmentsFinished();
        }
    }
}

void AttachmentsAdder::doneAddingAttachments()
{
    // Unlocking must happen from the same thread as locking (says Windows)
    draftModel_->addingAttachmentsInProgress().unlock();
    working_.unlock();
}

bool AttachmentsAdder::addNextAttachment()
{
    if (attachmentsToBeAdded_.empty()) return false;

    currentlyAddedAttachment_ = attachmentsToBeAdded_.front();
    attachmentsToBeAdded_.pop_front();
    draftModel_->addAttachment(currentlyAddedAttachment_->source);
    return true;
}

}
}
