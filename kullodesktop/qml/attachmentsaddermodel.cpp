/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#include "attachmentsaddermodel.h"

#include <QQmlEngine>
#include <desktoputil/asynctask.h>
#include <kulloclient/util/assert.h>

#include "kullodesktop/qml/draftmodel.h"

namespace KulloDesktop {
namespace Qml {

AttachmentsAdderModel::AttachmentsAdderModel(QObject *parent) :
    QObject(parent),
    cancel_(false)
{
    connect(this, &AttachmentsAdderModel::_doneAddingAttachments,
            this, &AttachmentsAdderModel::onDoneAddingAttachments);
}

AttachmentsAdderModel::~AttachmentsAdderModel()
{
    cancel_ = true;
    if (backgroundJob_.joinable()) backgroundJob_.join();
}

DraftModel *AttachmentsAdderModel::target() const
{
    QQmlEngine::setObjectOwnership(draftModel_, QQmlEngine::CppOwnership);
    return draftModel_;
}

void AttachmentsAdderModel::setTarget(DraftModel *target)
{
    if (draftModel_ != target)
    {
        draftModel_ = target;
        emit targetChanged();
    }
}

bool AttachmentsAdderModel::addAttachments(const QList<QUrl> &urls)
{
    if (!working_.try_lock()) return false;

    if (backgroundJob_.joinable()) backgroundJob_.join();

    cancel_ = false;
    kulloAssert(draftModel_);

    // prevent DraftModel from being deleted as long as we are working
    draftModel_->addingAttachmentsInProgress().lock();

    emit addingAttachmentsStarted();

    auto doAddAttachments = [&](DraftModel *draftModel, const QList<QUrl> &urls)
    {
        for (const auto &url : urls)
        {
            if (cancel_) break;
            draftModel->addAttachment(url);
        }
        emit _doneAddingAttachments();
    };
    backgroundJob_ = DesktopUtil::AsyncTask::run(std::bind(doAddAttachments, draftModel_, urls));

    return true;
}

void AttachmentsAdderModel::onDoneAddingAttachments()
{
    // Unlocking must happen from the same thread as locking (says Windows)
    draftModel_->addingAttachmentsInProgress().unlock();
    working_.unlock();

    emit addingAttachmentsFinished();
}

}
}
