/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <QObject>

#include <kulloclient/api/MessageAttachmentsContentListener.h>

namespace ApiMirror {

class MessageAttachmentsContentListener
        : public QObject
        , public Kullo::Api::MessageAttachmentsContentListener
{
    Q_OBJECT

public:
    explicit MessageAttachmentsContentListener(QObject *parent = nullptr)
        : QObject(parent)
    {}

    void finished(int64_t msgId, int64_t attId, const std::vector<uint8_t> &content) override
    {
        emit _finished(msgId, attId, content);
    }

signals:
    void _finished(int64_t msgId, int64_t attId, const std::vector<uint8_t> &content);
};

}
