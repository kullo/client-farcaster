/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#include "registerer.h"

#include <kulloclient/api/Address.h>
#include <kulloclient/api/AddressNotAvailableReason.h>
#include <kulloclient/api/AsyncTask.h>
#include <kulloclient/api/Client.h>
#include <kulloclient/api/ClientGenerateKeysListener.h>
#include <kulloclient/api/MasterKey.h>
#include <kulloclient/api/Registration.h>
#include <kulloclient/api/RegistrationRegisterAccountListener.h>
#include <kulloclient/util/assert.h>
#include <kulloclient/util/librarylogger.h>
#include <kulloclient/util/misc.h>

namespace {

class GenKeysListener: public Kullo::Api::ClientGenerateKeysListener
{
public:
    void progress(int8_t progress) override
    {
        Log.i() << "Key generation: "  << std::to_string(progress) << " %";
    }

    void finished(const std::shared_ptr<Kullo::Api::Registration> &registration) override
    {
        registration_ = registration;
    }

    std::shared_ptr<Kullo::Api::Registration> registration() const
    {
        return registration_;
    }

private:
    std::shared_ptr<Kullo::Api::Registration> registration_;
};

class RegAccListener: public Kullo::Api::RegistrationRegisterAccountListener
{
public:
    void challengeNeeded(
            const std::shared_ptr<Kullo::Api::Address> &address,
            const std::shared_ptr<Kullo::Api::Challenge> &challenge) override
    {
        K_UNUSED(address);
        K_UNUSED(challenge);
        Log.f() << "Challenge requested, but challenges are not implemented.";
    }

    void addressNotAvailable(
            const std::shared_ptr<Kullo::Api::Address> &address,
            Kullo::Api::AddressNotAvailableReason reason) override
    {
        //FIXME log reason
        K_UNUSED(reason);
        Log.f() << "Address " << address->toString() << " not available.";
    }

    void finished(
            const std::shared_ptr<Kullo::Api::Address> &address,
            const std::shared_ptr<Kullo::Api::MasterKey> &masterKey) override
    {
        Log.i() << "Successfully registered address " << address->toString();
        masterKey_ = masterKey;
    }

    void error(
            const std::shared_ptr<Kullo::Api::Address> &address,
            Kullo::Api::NetworkError error) override
    {
        K_UNUSED(address);
        //FIXME log error
        K_UNUSED(error);
        Log.f() << "Error while registering address.";
    }

    std::shared_ptr<Kullo::Api::MasterKey> masterKey() const
    {
        return masterKey_;
    }

private:
    std::shared_ptr<Kullo::Api::MasterKey> masterKey_;
};

}

std::shared_ptr<Kullo::Api::MasterKey> Registerer::run(
        const std::shared_ptr<Kullo::Api::Address> &address)
{
    auto client = Kullo::Api::Client::create();

    auto genKeysL = std::make_shared<GenKeysListener>();
    client->generateKeysAsync(genKeysL)->waitUntilDone();

    auto registration = genKeysL->registration();
    kulloAssert(registration);
    auto regAccL = std::make_shared<RegAccListener>();
    registration->
            registerAccountAsync(address, nullptr, "", regAccL)->
            waitUntilDone();

    return regAccL->masterKey();
}
