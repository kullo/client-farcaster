/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#include "registerer.h"

#include <fstream>

#include <kulloclient/util/assert.h>
#include <kulloclient/util/kulloaddress.h>
#include <kulloclient/util/librarylogger.h>
#include <kulloclient/util/masterkey.h>
#include <kulloclient/util/misc.h>
#include <kulloclient/util/formatstring.h>

Registerer::Registerer(const QCoreApplication &app, QObject *parent)
    : QObject(parent)
    , app_(app)
{

}

Registerer::~Registerer()
{
}

void Registerer::run(const Kullo::Util::KulloAddress &address, const Kullo::Util::MasterKey &masterKey)
{
    registration_ = Kullo::make_unique<Kullo::Model::Registration>();
    registrationAddress_ = Kullo::make_unique<Kullo::Util::KulloAddress>(address);

    connect(registration_.get(), &Kullo::Model::Registration::keysGenerationProgressChanged,
            this, &Registerer::onKeysGenerationProgressChanged);
    connect(registration_.get(), &Kullo::Model::Registration::keysGenerationDone,
            this, &Registerer::onKeysGenerationDone);

    connect(registration_.get(), &Kullo::Model::Registration::success,
            this, &Registerer::onSuccess);
    connect(registration_.get(), &Kullo::Model::Registration::error,
            this, &Registerer::onError);

    connect(registration_.get(), &Kullo::Model::Registration::codeChallenge,
            this, &Registerer::onChallenge);
    connect(registration_.get(), &Kullo::Model::Registration::reservationChallenge,
            this, &Registerer::onChallenge);
    connect(registration_.get(), &Kullo::Model::Registration::resetChallenge,
            this, &Registerer::onChallenge);
    connect(registration_.get(), &Kullo::Model::Registration::addressExists,
            this, &Registerer::onAddressExists);
    connect(registration_.get(), &Kullo::Model::Registration::addressBlocked,
            this, &Registerer::onAddressBlocked);

    registration_->genKeys(masterKey);
}

std::string Registerer::getStringContentsOrCrash(const std::string &filename, bool trimmed)
{
    std::string out;

    try {
        std::ifstream t(filename);

        if (t.fail())
        {
            Log.e() << "Opening file failed: " << filename;
            exit(1);
        }

        out = std::string(std::istreambuf_iterator<char>{t},
                          std::istreambuf_iterator<char>{});
    }
    catch(std::exception& e)
    {
        Log.f() << e.what();
    }

    if (trimmed)
    {
        Kullo::Util::FormatString::trim(out);
    }

    return out;
}

void Registerer::onKeysGenerationProgressChanged(int progress)
{
    Log.i() << "Key generation: "  << progress << " %";
}

void Registerer::onKeysGenerationDone()
{
    Log.i() << "MasterKey:\n"
            << registration_->userSettings().masterKey->toPem();

    kulloAssert(registrationAddress_);

    registration_->registerAccount(*registrationAddress_);
}

void Registerer::onSuccess()
{
    Log.i() << "Registration succeeded";
    app_.exit(0);
}

void Registerer::onChallenge()
{
    Log.f() << "Challenge received. Cannot handle challenged. Will now crash, Cya!";
}

void Registerer::onAddressExists()
{
    Log.f() << "Address exists. Cannot handle this. Will now crash, Cya!";
}

void Registerer::onAddressBlocked()
{
    Log.f() << "Address blocked. Cannot handle this. Will now crash, Cya!";
}

void Registerer::onError(std::exception_ptr exception)
{
    if (exception) std::rethrow_exception(exception);
    app_.exit(1);
}
