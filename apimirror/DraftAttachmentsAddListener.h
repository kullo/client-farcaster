/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <QObject>

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
        qRegisterMetaType<Kullo::Api::LocalError>("Kullo::Api::LocalError");
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
    void _finished(int64_t convId, int64_t attId, const std::string &path);
    void _error(int64_t convId, const std::string &path, Kullo::Api::LocalError error);
};

}
