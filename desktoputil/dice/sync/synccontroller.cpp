/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#include "synccontroller.h"

#include <kulloclient/util/assert.h>

#include "desktoputil/dice/sync/syncerqobject.h"

namespace Kullo {
namespace Sync {

SyncController::SyncController(Model::Client *client, QObject *parent)
    : QObject(parent)
    , shouldCancel_(std::make_shared<std::atomic<bool>>(false))
    , client_(client)
{
    kulloAssert(client_);
    syncThread_.setObjectName("Kullo sync thread");
}

SyncController::~SyncController()
{
    cancelSync();
}

bool SyncController::sync(SyncMode syncMode)
{
    if (!syncMutex_.try_lock()) {
        return false;
    }

    shouldCancel_ = std::make_shared<std::atomic<bool>>(false);

    SyncerQObject *syncer = new SyncerQObject(
                client_->dbFile(),
                client_->userSettings(),
                shouldCancel_);
    syncer->moveToThread(&syncThread_);

    // delete syncer when thread finishes
    connect(&syncThread_, &QThread::finished,
            syncer, &SyncerQObject::deleteLater);

    connect(syncer, &SyncerQObject::progressed,
            this, &SyncController::onSyncProgressed);
    connect(syncer, &SyncerQObject::finished,
            this, &SyncController::onSyncFinished);
    connect(syncer, &SyncerQObject::error,
            this, &SyncController::onSyncError);

    // Forward signals: Syncer -> SyncController
    connect(syncer, &SyncerQObject::draftModified,
            this, &SyncController::draftModified);
    connect(syncer, &SyncerQObject::draftAttachmentsTooBig,
            this, &SyncController::draftAttachmentsTooBig);
    connect(syncer, &SyncerQObject::participantAddedOrModified,
            this, &SyncController::participantAddedOrModified);
    connect(syncer, &SyncerQObject::messageAdded,
            this, &SyncController::messageAdded);
    connect(syncer, &SyncerQObject::messageDeleted,
            this, &SyncController::messageDeleted);
    connect(syncer, &SyncerQObject::messageModified,
            this, &SyncController::messageModified);
    connect(syncer, &SyncerQObject::messageAttachmentsDownloaded,
            this, &SyncController::messageAttachmentsDownloaded);
    connect(syncer, &SyncerQObject::conversationModified,
            this, &SyncController::conversationModified);
    connect(syncer, &SyncerQObject::conversationAdded,
            this, &SyncController::conversationAdded);

    // start the thread's event loop
    syncThread_.start();

    // start syncing
    emit syncer->run(syncMode);

    return true;
}

void SyncController::cancelSync()
{
    *shouldCancel_ = true;
    if (syncThread_.isRunning()) tearDownSyncThread();
}

void SyncController::onSyncProgressed(const SyncProgress &progress)
{
    emit syncProgressed(progress);
}

void SyncController::onSyncFinished(const SyncProgress &progress)
{
    tearDownSyncThread();
    emit syncFinished(progress);
}

void SyncController::onSyncError(std::exception_ptr exptr)
{
    tearDownSyncThread();
    emit syncError(exptr);
}

void SyncController::tearDownSyncThread()
{
    syncThread_.quit();
    syncThread_.wait();

    // after this procedure, the mutex is guaranteed to be unlocked
    syncMutex_.try_lock();
    syncMutex_.unlock();
}

}
}
