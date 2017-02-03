/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <memory>
#include <QObject>

#include <kulloclient/types.h>
#include <kulloclient/api/SyncerListener.h>
#include <kulloclient/api/SyncProgress.h>

#include "apimirror/enums/DraftParts.h"

namespace ApiMirror {

class SyncerListener : public QObject, public Kullo::Api::SyncerListener
{
    Q_OBJECT

public:
    explicit SyncerListener(QObject *parent = nullptr)
        : QObject(parent)
    {
        // registered in registerMetaTypes(): Kullo::id_type
        qRegisterMetaType<std::shared_ptr<Kullo::Api::SyncProgress>>("std::shared_ptr<Kullo::Api::SyncProgress>");
        qRegisterMetaType<Kullo::Api::NetworkError>("Kullo::Api::NetworkError");
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
                              ApiMirror::Enums::DraftParts::convert(part),
                              currentSize, maxSize);
    }

    void progressed(const Kullo::Api::SyncProgress &progress) override
    {
        emit _progressed(std::make_shared<Kullo::Api::SyncProgress>(progress));
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
            Kullo::id_type convId, ApiMirror::Enums::DraftParts::DraftPart part,
            int64_t currentSize, int64_t maxSize);
    void _progressed(const std::shared_ptr<Kullo::Api::SyncProgress> &progress);
    void _finished();
    void _error(Kullo::Api::NetworkError error);
};

}
