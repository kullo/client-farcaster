/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <QObject>

#include <kulloclient/api/DraftAttachmentsContentListener.h>

namespace ApiMirror {

class DraftAttachmentsContentListener
        : public QObject
        , public Kullo::Api::DraftAttachmentsContentListener
{
    Q_OBJECT

public:
    explicit DraftAttachmentsContentListener(QObject *parent = nullptr)
        : QObject(parent)
    {}

    void finished(int64_t convId, int64_t attId, const std::vector<uint8_t> &content) override
    {
        emit _finished(convId, attId, content);
    }

signals:
    void _finished(int64_t convId, int64_t attId, const std::vector<uint8_t> &content);
};

}
