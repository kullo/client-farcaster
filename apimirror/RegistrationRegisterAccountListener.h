/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#pragma once

#include <QObject>

#include <kulloclient/api/RegistrationRegisterAccountListener.h>
#include <kulloclient/api_impl/Address.h>
#include <kulloclient/api_impl/MasterKey.h>

#include "apimirror/misc.h"
#include "apimirror/signalslotvalue.h"

namespace ApiMirror {

class RegistrationRegisterAccountListener
        : public QObject
        , public Kullo::Api::RegistrationRegisterAccountListener
{
    Q_OBJECT

public:
    explicit RegistrationRegisterAccountListener(QObject *parent = 0)
        : QObject(parent)
    {
        K_REGISTER_QT_META_TYPE(ApiMirror::SignalSlotValue<Kullo::Api::Address>);
        K_REGISTER_QT_META_TYPE(ApiMirror::SignalSlotValue<Kullo::Api::MasterKey>);
        K_REGISTER_QT_META_TYPE(std::shared_ptr<Kullo::Api::Challenge>);
        K_REGISTER_QT_META_TYPE(Kullo::Api::AddressNotAvailableReason);
        K_REGISTER_QT_META_TYPE(Kullo::Api::NetworkError);
    }

    void challengeNeeded(
            const Kullo::Api::Address &address,
            const Kullo::nn_shared_ptr<Kullo::Api::Challenge> &challenge) override
    {
        emit _challengeNeeded(address, challenge.as_nullable());
    }

    void addressNotAvailable(
            const Kullo::Api::Address &address,
            Kullo::Api::AddressNotAvailableReason reason) override
    {
        emit _addressNotAvailable(address, reason);
    }

    void finished(
            const Kullo::Api::Address &address,
            const Kullo::Api::MasterKey &masterKey) override
    {
        emit _finished(address, masterKey);
    }

    void error(
            const Kullo::Api::Address &address,
            Kullo::Api::NetworkError error) override
    {
        emit _error(address, error);
    }

signals:
    void _challengeNeeded(const SignalSlotValue<Kullo::Api::Address> &address, const std::shared_ptr<Kullo::Api::Challenge> &challenge);
    void _addressNotAvailable(const SignalSlotValue<Kullo::Api::Address> &address, Kullo::Api::AddressNotAvailableReason reason);
    void _finished(const SignalSlotValue<Kullo::Api::Address> &address, const SignalSlotValue<Kullo::Api::MasterKey> &masterKey);
    void _error(const SignalSlotValue<Kullo::Api::Address> &address, Kullo::Api::NetworkError error);

};

}
