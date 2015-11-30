/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#pragma once

#include <atomic>
#include <chrono>
#include <memory>
#include <QObject>

#include <kulloclient/kulloclient-forwards.h>
#include <kulloclient/types.h>
#include <kulloclient/sync/definitions.h>
#include <kulloclient/sync/syncer.h>
#include <kulloclient/util/usersettings.h>
#include <kulloclient/util/misc.h>

namespace Kullo {
namespace Sync {

/**
 * @brief Syncer that coordinates all other syncers.
 */
class SyncerQObject : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Creates new syncer.
     * @param dbFile The SQLite DB file that should be used.
     * @param settings Settings for the local user.
     */
    explicit SyncerQObject(
            const std::string &dbFile,
            const Util::UserSettings &settings,
            std::shared_ptr<std::atomic<bool>> shouldCancel);
    ~SyncerQObject();

signals:
    /**
     * @brief The syncer listens to this signal and starts syncing when it is emitted.
     */
    void run(Kullo::Sync::SyncMode syncMode);

    ///  Emitted when the syncer has made progress
    void progressed(const Kullo::Sync::SyncProgress &progress);

    ///  Emitted when the syncer has finished successfully.
    void finished(const Kullo::Sync::SyncProgress &progress);

    ///  Emitted when the syncer has finished unsuccessfully.
    void error(std::exception_ptr exptr);

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
    void onRun(SyncMode syncMode);

private:
    std::shared_ptr<std::atomic<bool>> shouldCancel_;
    Syncer syncer_;

    K_DISABLE_COPY(SyncerQObject);
};

}
}
