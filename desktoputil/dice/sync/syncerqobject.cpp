/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#include "desktoputil/dice/sync/syncerqobject.h"

#include <kulloclient/codec/privatekeyprovider.h>
#include <kulloclient/util/librarylogger.h>

using namespace Kullo::Util;

#define KULLO_BEGIN_EMIT_ERROR_ON_EXCEPTION try {
#define KULLO_END_EMIT_ERROR_ON_EXCEPTION \
    } catch (...) { emit error(std::current_exception()); }

namespace Kullo {
namespace Sync {

SyncerQObject::SyncerQObject(
        const std::string &dbFile,
        const UserSettings &settings,
        std::shared_ptr<std::atomic<bool>> shouldCancel)
    : shouldCancel_(shouldCancel)
    , syncer_(dbFile, settings,
              std::make_shared<Codec::PrivateKeyProvider>(dbFile))
{
    /* If onRun() would be called directly from the Client, it would be
     * executed in the caller thread. However, Client and Syncer live in
     * different threads (see Client::sync()), so we use a signal that uses
     * the Qt::QueuedConnection behavior to be delivered to the receiver.
     *
     * Thus, run() is emitted from the Client thread, while onRun() is
     * executed in the Syncer thread.
     */
    connect(this, &SyncerQObject::run, this, &SyncerQObject::onRun);
}

SyncerQObject::~SyncerQObject()
{
}

void SyncerQObject::onRun(Kullo::Sync::SyncMode syncMode)
{
    KULLO_BEGIN_EMIT_ERROR_ON_EXCEPTION;

    Util::LibraryLogger::setCurrentThreadName("sync");

    // connect modification signals
    syncer_.events.progressed =
            [this](SyncProgress progress)
    {
        emit progressed(progress);
    };
    syncer_.events.finished =
            [this](SyncProgress progress)
    {
        emit finished(progress);
    };
    syncer_.events.participantAddedOrModified =
            [this](std::string address)
    {
        emit participantAddedOrModified(address);
    };
    syncer_.events.conversationAdded =
            [this](id_type conversationId)
    {
        emit conversationAdded(conversationId);
    };
    syncer_.events.conversationModified =
            [this](id_type conversationId)
    {
        emit conversationModified(conversationId);
    };
    syncer_.events.draftModified =
            [this](id_type conversationId)
    {
        emit draftModified(conversationId);
    };
    syncer_.events.messageAdded =
            [this](id_type conversationId, id_type messageId)
    {
        emit messageAdded(conversationId, messageId);
    };
    syncer_.events.messageModified =
            [this](id_type conversationId, id_type messageId)
    {
        emit messageModified(conversationId, messageId);
    };
    syncer_.events.messageDeleted =
            [this](Kullo::id_type conversationId, Kullo::id_type id)
    {
        emit messageDeleted(conversationId, id);
    };
    syncer_.events.messageAttachmentsDownloaded =
            [this](id_type conversationId, id_type messageId)
    {
        emit messageAttachmentsDownloaded(conversationId, messageId);
    };
    syncer_.events.draftAttachmentsTooBig =
            [this](
            id_type conversationId,
            std::size_t size,
            std::size_t sizeAllowed)
    {
        emit draftAttachmentsTooBig(conversationId, size, sizeAllowed);
    };

    // run syncers
    syncer_.run(syncMode, shouldCancel_);

    KULLO_END_EMIT_ERROR_ON_EXCEPTION;
}

}
}
