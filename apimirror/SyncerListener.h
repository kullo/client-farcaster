/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#pragma once

#include <memory>
#include <QObject>

#include <kulloclient/types.h>
#include <kulloclient/api/SyncerListener.h>
#include <kulloclient/api/SyncProgress.h>

#include "apimirror/enums/DraftPartHolder.h"
#include "apimirror/misc.h"
#include "apimirror/signalslotvalue.h"

namespace ApiMirror {

class SyncerListener : public QObject, public Kullo::Api::SyncerListener
{
    Q_OBJECT

public:
    explicit SyncerListener(QObject *parent = nullptr)
        : QObject(parent)
    {
        // registered in registerMetaTypes(): Kullo::id_type
        K_REGISTER_QT_META_TYPE(Kullo::Api::NetworkError);
        K_REGISTER_QT_META_TYPE(SignalSlotValue<Kullo::Api::SyncProgress>);
    }

    void started() override
    {
        emit _started();
    }

    void draftPartTooBig(
            int64_t convId, Kullo::Api::DraftPart part,
            int64_t currentSize, int64_t maxSize) override
    {
        emit _draftPartTooBig(convId,
                              ApiMirror::Enums::DraftPartHolder::convert(part),
                              currentSize, maxSize);
    }

    void progressed(const Kullo::Api::SyncProgress &progress) override
    {
        emit _progressed(progress);
    }

    void finished() override
    {
        emit _finished();
    }

    void error(Kullo::Api::NetworkError error) override
    {
        emit _error(error);
    }

signals:
    void _started();
    void _draftPartTooBig(
            Kullo::id_type convId,
            ApiMirror::Enums::DraftPartHolder::DraftPart part,
            int64_t currentSize, int64_t maxSize);
    void _progressed(const SignalSlotValue<Kullo::Api::SyncProgress> &progress);
    void _finished();
    void _error(Kullo::Api::NetworkError error);
};

}
