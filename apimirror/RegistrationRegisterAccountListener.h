/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <QObject>

#include <kulloclient/api/RegistrationRegisterAccountListener.h>

namespace ApiMirror {

class RegistrationRegisterAccountListener
        : public QObject
        , public Kullo::Api::RegistrationRegisterAccountListener
{
    Q_OBJECT

public:
    explicit RegistrationRegisterAccountListener(QObject *parent = 0) : QObject(parent)
    {
        qRegisterMetaType<std::shared_ptr<Kullo::Api::Address>>("std::shared_ptr<Kullo::Api::Address>");
        qRegisterMetaType<std::shared_ptr<Kullo::Api::Challenge>>("std::shared_ptr<Kullo::Api::Challenge>");
        qRegisterMetaType<std::shared_ptr<Kullo::Api::MasterKey>>("std::shared_ptr<Kullo::Api::MasterKey>");
        qRegisterMetaType<Kullo::Api::AddressNotAvailableReason>("Kullo::Api::AddressNotAvailableReason");
        qRegisterMetaType<Kullo::Api::NetworkError>("Kullo::Api::NetworkError");
    }

    void challengeNeeded(const std::shared_ptr<Kullo::Api::Address> & address, const std::shared_ptr<Kullo::Api::Challenge> & challenge) override
    {
        emit _challengeNeeded(address, challenge);
    }

    void addressNotAvailable(const std::shared_ptr<Kullo::Api::Address> & address, Kullo::Api::AddressNotAvailableReason reason) override
    {
        emit _addressNotAvailable(address, reason);
    }

    void finished(const std::shared_ptr<Kullo::Api::Address> & address, const std::shared_ptr<Kullo::Api::MasterKey> & masterKey) override
    {
        emit _finished(address, masterKey);
    }

    void error(const std::shared_ptr<Kullo::Api::Address> & address, Kullo::Api::NetworkError error) override
    {
        emit _error(address, error);
    }

signals:
    void _challengeNeeded(const std::shared_ptr<Kullo::Api::Address> & address, const std::shared_ptr<Kullo::Api::Challenge> & challenge);
    void _addressNotAvailable(const std::shared_ptr<Kullo::Api::Address> & address, Kullo::Api::AddressNotAvailableReason reason);
    void _finished(const std::shared_ptr<Kullo::Api::Address> & address, const std::shared_ptr<Kullo::Api::MasterKey> & masterKey);
    void _error(const std::shared_ptr<Kullo::Api::Address> & address, Kullo::Api::NetworkError error);

};

}
