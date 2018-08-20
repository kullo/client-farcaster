/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
#include "registerer.h"

#include <boost/optional.hpp>

#include <kulloclient/api/AddressNotAvailableReason.h>
#include <kulloclient/api/AsyncTask.h>
#include <kulloclient/api/Client.h>
#include <kulloclient/api/ClientGenerateKeysListener.h>
#include <kulloclient/api/Registration.h>
#include <kulloclient/api/RegistrationRegisterAccountListener.h>
#include <kulloclient/api_impl/Address.h>
#include <kulloclient/api_impl/MasterKey.h>
#include <kulloclient/util/assert.h>
#include <kulloclient/util/librarylogger.h>
#include <kulloclient/util/misc.h>

namespace {

class GenKeysListener: public Kullo::Api::ClientGenerateKeysListener
{
public:
    void progress(int8_t progress) override
    {
        Log.i() << "Key generation: "  << static_cast<int>(progress) << " %";
    }

    void finished(const Kullo::nn_shared_ptr<Kullo::Api::Registration> &registration) override
    {
        registration_ = registration.as_nullable();
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
            const Kullo::Api::Address &address,
            const Kullo::nn_shared_ptr<Kullo::Api::Challenge> &challenge) override
    {
        K_UNUSED(address);
        K_UNUSED(challenge);
        Log.f() << "Challenge requested, but challenges are not implemented.";
    }

    void addressNotAvailable(
            const Kullo::Api::Address &address,
            Kullo::Api::AddressNotAvailableReason reason) override
    {
        //FIXME log reason
        K_UNUSED(reason);
        Log.f() << "Address " << address << " not available.";
    }

    void finished(
            const Kullo::Api::Address &address,
            const Kullo::Api::MasterKey &masterKey) override
    {
        Log.i() << "Successfully registered address " << address;
        masterKey_ = masterKey;
    }

    void error(
            const Kullo::Api::Address &address,
            Kullo::Api::NetworkError error) override
    {
        K_UNUSED(address);
        //FIXME log error
        K_UNUSED(error);
        Log.f() << "Error while registering address.";
    }

    boost::optional<Kullo::Api::MasterKey> masterKey() const
    {
        return masterKey_;
    }

private:
    boost::optional<Kullo::Api::MasterKey> masterKey_;
};

}

Kullo::Api::MasterKey Registerer::run(
        const Kullo::Api::Address &address)
{
    auto client = Kullo::Api::Client::create();

    auto genKeysL = Kullo::nn_make_shared<GenKeysListener>();
    client->generateKeysAsync(genKeysL)->waitUntilDone();

    auto registration = genKeysL->registration();
    kulloAssert(registration);
    auto regAccL = Kullo::nn_make_shared<RegAccListener>();
    registration->registerAccountAsync(
                address,
                boost::none, // terms
                nullptr, // challenge
                "", // challenge answer
                regAccL)->
            waitUntilDone();

    return *regAccL->masterKey();
}
