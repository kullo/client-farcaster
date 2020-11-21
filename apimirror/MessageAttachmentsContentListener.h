/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#pragma once

#include <QObject>

#include <kulloclient/types.h>
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
    {
        // registered in registerMetaTypes(): Kullo::id_type
        // why does std::vector<uint8_t> work?
    }

    void finished(int64_t msgId, int64_t attId, const std::vector<uint8_t> &content) override
    {
        emit _finished(msgId, attId, content);
    }

signals:
    void _finished(Kullo::id_type msgId, Kullo::id_type attId, const std::vector<uint8_t> &content);
};

}
