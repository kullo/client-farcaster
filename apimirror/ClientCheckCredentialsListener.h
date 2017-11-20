/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#pragma once

#include <QObject>

#include <kulloclient/api/ClientCheckCredentialsListener.h>
#include <kulloclient/api_impl/Address.h>
#include <kulloclient/api_impl/MasterKey.h>

#include "apimirror/misc.h"
#include "apimirror/signalslotvalue.h"

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
        K_REGISTER_QT_META_TYPE(SignalSlotValue<Kullo::Api::Address>);
        K_REGISTER_QT_META_TYPE(SignalSlotValue<Kullo::Api::MasterKey>);
        K_REGISTER_QT_META_TYPE(Kullo::Api::NetworkError);
    }

    void finished(
            const Kullo::Api::Address &address,
            const Kullo::Api::MasterKey &masterKey,
            bool exists) override
    {
        emit _finished(address, masterKey, exists);
    }

    void error(
            const Kullo::Api::Address &address,
            Kullo::Api::NetworkError e) override
    {
        emit _error(address, e);
    }

signals:
    void _finished(
            const SignalSlotValue<Kullo::Api::Address> &address,
            const SignalSlotValue<Kullo::Api::MasterKey> &masterKey,
            bool exists);
    void _error(
            const SignalSlotValue<Kullo::Api::Address> &address,
            Kullo::Api::NetworkError error);
};

}
