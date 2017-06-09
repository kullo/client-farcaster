/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#pragma once

#include <QObject>

#include <kulloclient/api/Address.h>
#include <kulloclient/api/ClientCheckCredentialsListener.h>

#include "apimirror/misc.h"

namespace ApiMirror {

class ClientCheckCredentialsListener
        : public QObject
        , public Kullo::Api::ClientCheckCredentialsListener
{
    Q_OBJECT

public:
    explicit ClientCheckCredentialsListener(QObject *parent = nullptr)
        : QObject(parent)
    {
        // no registration needed for: bool
        K_REGISTER_QT_META_TYPE(std::shared_ptr<Kullo::Api::Address>);
        K_REGISTER_QT_META_TYPE(std::shared_ptr<Kullo::Api::MasterKey>);
        K_REGISTER_QT_META_TYPE(Kullo::Api::NetworkError);
    }

    void finished(
            const std::shared_ptr<Kullo::Api::Address> &address,
            const std::shared_ptr<Kullo::Api::MasterKey> &masterKey,
            bool exists) override
    {
        emit _finished(address, masterKey, exists);
    }

    void error(
            const std::shared_ptr<Kullo::Api::Address> &address,
            Kullo::Api::NetworkError e) override
    {
        emit _error(address, e);
    }

signals:
    void _finished(
            const std::shared_ptr<Kullo::Api::Address> &address,
            const std::shared_ptr<Kullo::Api::MasterKey> &masterKey,
            bool exists);
    void _error(
            const std::shared_ptr<Kullo::Api::Address> &address,
            Kullo::Api::NetworkError error);
};

}
