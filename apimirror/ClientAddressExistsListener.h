/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#pragma once

#include <QObject>

#include <kulloclient/api/ClientAddressExistsListener.h>
#include <kulloclient/api_impl/Address.h>

#include "apimirror/misc.h"

namespace ApiMirror {

class ClientAddressExistsListener
        : public QObject
        , public Kullo::Api::ClientAddressExistsListener
{
    Q_OBJECT

public:
    explicit ClientAddressExistsListener(QObject *parent = nullptr)
        : QObject(parent)
    {
        // no registration needed for: bool
        // registered in registerMetaTypes(): std::string
        K_REGISTER_QT_META_TYPE(Kullo::Api::NetworkError);
    }

    void finished(const Kullo::Api::Address &address, bool exists) override
    {
        emit _finished(address.toString(), exists);
    }

    void error(const Kullo::Api::Address &address, Kullo::Api::NetworkError e) override
    {
        emit _error(address.toString(), e);
    }

signals:
    void _finished(std::string address, bool exists);
    void _error(std::string address, Kullo::Api::NetworkError error);
};

}
