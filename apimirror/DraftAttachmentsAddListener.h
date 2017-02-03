/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#pragma once

#include <QObject>

#include <kulloclient/types.h>
#include <kulloclient/api/DraftAttachmentsAddListener.h>

namespace ApiMirror {

class DraftAttachmentsAddListener
        : public QObject
        , public Kullo::Api::DraftAttachmentsAddListener
{
    Q_OBJECT

public:
    explicit DraftAttachmentsAddListener(QObject *parent = nullptr)
        : QObject(parent)
    {
        // registered in registerMetaTypes(): Kullo::id_type, std::string
        qRegisterMetaType<Kullo::Api::LocalError>("Kullo::Api::LocalError");
    }

    void progressed(int64_t convId, int64_t attId, int64_t bytesProcessed, int64_t bytesTotal) override
    {
        emit _progressed(convId, attId, bytesProcessed, bytesTotal);
    }

    void finished(int64_t convId, int64_t attId, const std::string &path) override
    {
        emit _finished(convId, attId, path);
    }

    void error(int64_t convId, const std::string &path, Kullo::Api::LocalError error) override
    {
        emit _error(convId, path, error);
    }

signals:
    void _progressed(Kullo::id_type convId, Kullo::id_type attId, int64_t bytesProcessed, int64_t bytesTotal);
    void _finished(Kullo::id_type convId, Kullo::id_type attId, const std::string &path);
    void _error(Kullo::id_type convId, const std::string &path, Kullo::Api::LocalError error);
};

}
