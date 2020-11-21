/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#pragma once

#include <QObject>

#include <kulloclient/types.h>
#include <kulloclient/api/MessageAttachmentsSaveToListener.h>

#include "apimirror/misc.h"

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
        K_REGISTER_QT_META_TYPE(Kullo::Api::LocalError);
    }

    void finished(int64_t msgId, int64_t attId, const std::string &path) override
    {
        emit _finished(msgId, attId, path);
    }

    void error(int64_t msgId, int64_t attId, const std::string &path, Kullo::Api::LocalError error) override
    {
        emit _error(msgId, attId, path, error);
    }

signals:
    void _finished(Kullo::id_type msgId, Kullo::id_type attId, const std::string &path);
    void _error(Kullo::id_type msgId, Kullo::id_type attId, const std::string &path, Kullo::Api::LocalError error);
};

}
