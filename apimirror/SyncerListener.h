/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <memory>
#include <QObject>

#include <kulloclient/api/SyncerListener.h>
#include <kulloclient/api/SyncProgress.h>

namespace ApiMirror {

class SyncerListener : public QObject, public Kullo::Api::SyncerListener
{
    Q_OBJECT
public:
    explicit SyncerListener(QObject *parent = nullptr)
        : QObject(parent)
    {
        qRegisterMetaType<std::shared_ptr<Kullo::Api::SyncProgress>>("std::shared_ptr<Kullo::Api::SyncProgress>");
        qRegisterMetaType<Kullo::Api::NetworkError>("Kullo::Api::NetworkError");
    }

    void started() override
    {
        emit _started();
    }

    void draftAttachmentsTooBig(int64_t convId) override
    {
        emit _draftAttachmentsTooBig(convId);
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

    void _draftAttachmentsTooBig(int64_t convId);

    void _progressed(const std::shared_ptr<Kullo::Api::SyncProgress> &progress);

    void _finished();

    void _error(Kullo::Api::NetworkError error);
};

}
