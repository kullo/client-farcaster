/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#pragma once

#include <atomic>
#include <exception>
#include <mutex>
#include <QObject>
#include <QThread>

#include <kulloclient/types.h>
#include <kulloclient/sync/definitions.h>

#include "desktoputil/dice/model/client.h"

namespace Kullo {
namespace Sync {

class SyncController final : public QObject
{
    Q_OBJECT

public:
    explicit SyncController(Model::Client *client, QObject *parent = 0);
    ~SyncController();
    bool sync(SyncMode sendOnly);
    void cancelSync();

signals:
    void syncProgressed(const Kullo::Sync::SyncProgress &progress);
    void syncFinished(const Kullo::Sync::SyncProgress &progress);
    void syncError(std::exception_ptr exptr);

    void draftModified(Kullo::id_type conversationId);
    void draftAttachmentsTooBig(Kullo::id_type conversationId, std::size_t size, std::size_t sizeAllowed);
    void participantAddedOrModified(std::string address);
    void messageAdded(Kullo::id_type conversationId, Kullo::id_type messageId);
    void messageDeleted(Kullo::id_type conversationId, Kullo::id_type messageId);
    void messageModified(Kullo::id_type conversationId, Kullo::id_type id);
    void messageAttachmentsDownloaded(Kullo::id_type conversationId, Kullo::id_type messageId);
    void conversationModified(Kullo::id_type conversationId);
    void conversationAdded(Kullo::id_type conversationId);

private slots:
    void onSyncProgressed(const Kullo::Sync::SyncProgress &progress);
    void onSyncFinished(const Kullo::Sync::SyncProgress &progress);
    void onSyncError(std::exception_ptr exptr);

private:
    void tearDownSyncThread();

    std::shared_ptr<std::atomic<bool>> shouldCancel_;
    Model::Client *client_;
    std::mutex syncMutex_;
    QThread syncThread_;
};

}
}
