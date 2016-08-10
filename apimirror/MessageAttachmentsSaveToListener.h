/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <QObject>

#include <kulloclient/types.h>
#include <kulloclient/api/MessageAttachmentsSaveToListener.h>

namespace ApiMirror {

class MessageAttachmentsSaveToListener
        : public QObject
        , public Kullo::Api::MessageAttachmentsSaveToListener
{
    Q_OBJECT

public:
    explicit MessageAttachmentsSaveToListener(QObject *parent = nullptr)
        : QObject(parent)
    {
        // registered in registerMetaTypes(): Kullo::id_type, std::string
        qRegisterMetaType<Kullo::Api::LocalError>("Kullo::Api::LocalError");
    }

    void finished(int64_t msgId, int64_t attId, const std::string &path) override
    {
        emit _finished(Kullo::id_type{msgId}, Kullo::id_type{attId}, path);
    }

    void error(int64_t msgId, int64_t attId, const std::string &path, Kullo::Api::LocalError error) override
    {
        emit _error(Kullo::id_type{msgId}, Kullo::id_type{attId}, path, error);
    }

signals:
    void _finished(Kullo::id_type msgId, Kullo::id_type attId, const std::string &path);
    void _error(Kullo::id_type msgId, Kullo::id_type attId, const std::string &path, Kullo::Api::LocalError error);
};

}
