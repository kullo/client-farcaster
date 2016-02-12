/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#include "registerer.h"

#include <QQmlEngine>

#include <apimirror/Client.h>
#include <apimirror/ClientGenerateKeysListener.h>
#include <apimirror/RegistrationRegisterAccountListener.h>

#include <desktoputil/qtypestreamers.h>

#include <kulloclient/api/Address.h>
#include <kulloclient/api/AddressNotAvailableReason.h>
#include <kulloclient/api/Challenge.h>
#include <kulloclient/api/ChallengeType.h>
#include <kulloclient/api/Client.h>
#include <kulloclient/api/AsyncTask.h>
#include <kulloclient/api/MasterKey.h>
#include <kulloclient/api/NetworkError.h>
#include <kulloclient/api/Registration.h>

#include <kulloclient/util/assert.h>
#include <kulloclient/util/kulloaddress.h>
#include <kulloclient/util/librarylogger.h>
#include <kulloclient/util/misc.h>

namespace KulloDesktop {
namespace Qml {

void ChallengeTypes::init()
{
    qRegisterMetaType<ChallengeTypes::ChallengeType>("ChallengeTypes::ChallengeType");
}

Registerer::Registerer(QObject *parent)
    : QObject(parent)
{
    ChallengeTypes::init();

    listenerKeygen_ = std::make_shared<ApiMirror::ClientGenerateKeysListener>();

    connect(listenerKeygen_.get(), &ApiMirror::ClientGenerateKeysListener::_progress,
            this, &Registerer::keysGenerationProgressChanged);
    connect(listenerKeygen_.get(), &ApiMirror::ClientGenerateKeysListener::_finished,
            this, [this](const std::shared_ptr<Kullo::Api::Registration> & registration)
    {
        registration_ = registration;
    });

    listenerRegistration_ = std::make_shared<ApiMirror::RegistrationRegisterAccountListener>();

    connect(listenerRegistration_.get(), &ApiMirror::RegistrationRegisterAccountListener::_addressNotAvailable,
            this, [this](const std::shared_ptr<Kullo::Api::Address> & address, Kullo::Api::AddressNotAvailableReason reason)
    {
        auto addressString = QString::fromStdString(address->toString());

        if (reason == Kullo::Api::AddressNotAvailableReason::Blocked)
        {
            emit addressBlocked(addressString);
        }
        else if (reason == Kullo::Api::AddressNotAvailableReason::Exists)
        {
            emit addressExists(addressString);
        }
    });

    connect(listenerRegistration_.get(), &ApiMirror::RegistrationRegisterAccountListener::_challengeNeeded,
            this, [this](const std::shared_ptr<Kullo::Api::Address> & address, const std::shared_ptr<Kullo::Api::Challenge> & challenge)
    {
        lastChallenge_ = challenge;

        auto addressString = QString::fromStdString(address->toString());
        auto textString = QString::fromStdString(challenge->text());

        Log.i() << "Got challenge for " << addressString
                << ": " << textString;

        switch (challenge->type()) {
        case Kullo::Api::ChallengeType::Code:
            emit challengeNeeded(ChallengeTypes::Code, addressString, textString);
            break;
        case Kullo::Api::ChallengeType::Reservation:
            emit challengeNeeded(ChallengeTypes::Reservation, addressString, textString);
            break;
        case Kullo::Api::ChallengeType::Reset:
            emit challengeNeeded(ChallengeTypes::Reset, addressString, textString);
            break;
        default:
            break;
        }
    });

    connect(listenerRegistration_.get(), &ApiMirror::RegistrationRegisterAccountListener::_finished,
            this, [this](const std::shared_ptr<Kullo::Api::Address> & address, const std::shared_ptr<Kullo::Api::MasterKey> & masterKey)
    {
        auto addressString = QString::fromStdString(address->toString());
        auto masterKeyString = QString::fromStdString(masterKey->pem());
        emit succeeded(addressString, masterKeyString);
    });

    connect(listenerRegistration_.get(), &ApiMirror::RegistrationRegisterAccountListener::_error,
            this, [this](const std::shared_ptr<Kullo::Api::Address> & address, Kullo::Api::NetworkError error)
    {
        K_UNUSED(address);

        switch (error) {
        case Kullo::Api::NetworkError::Connection:
            emit networkError();
            break;
        case Kullo::Api::NetworkError::Protocol:
            emit protocolError();
            break;
        case Kullo::Api::NetworkError::Server:
            emit serverError();
            break;
        default:
            Log.f() << "Unhandled network error.";// << error;
        }
    });
}

Registerer::~Registerer()
{
}

ApiMirror::Client *Registerer::client() const
{
    QQmlEngine::setObjectOwnership(client_, QQmlEngine::CppOwnership);
    return client_;
}

void Registerer::setClient(ApiMirror::Client *client)
{
    kulloAssert(!client_);
    kulloAssert(client);
    client_ = client;
    emit clientChanged();
}

bool Registerer::locked() const
{
    return locked_;
}

void Registerer::setLocked(bool locked)
{
    if (locked_ == locked) return;

    locked_ = locked;
    emit lockedChanged();
}

void Registerer::generateKeys()
{
    // generateKeys() must be invoked only once per RegistererModel instance
    kulloAssert(!keyGenerationStarted_);
    keyGenerationStarted_ = true;

    taskKeygen_ = client_->raw()->generateKeysAsync(listenerKeygen_);
}

void Registerer::registerAccount(const QString &addr, const QString &challengeAnswer)
{
    kulloAssert(registration_);

    const std::string address = addr.trimmed().toStdString();
    const std::string answer = challengeAnswer.trimmed().toStdString();

    Log.i() << "Trying to register " << address << " ...";

    if (Kullo::Util::KulloAddress::isValid(address))
    {
        if (!answer.empty())
        {
            if (!lastChallenge_)
            {
                Log.f() << "Got challenge answer from users but no challenge is stored.";
            }

            Log.i() << "With challenge: " << lastChallenge_->text();
            taskRegister_ = registration_->registerAccountAsync(
                        Kullo::Api::Address::create(address),
                        lastChallenge_,
                        answer,
                        listenerRegistration_);
        }
        else
        {
            Log.i() << "No challenge used.";
            taskRegister_ = registration_->registerAccountAsync(
                        Kullo::Api::Address::create(address),
                        nullptr,
                        answer,
                        listenerRegistration_);
        }
    }
    else
    {
        emit invalidKulloAddress(QString::fromStdString(address));
    }
}

}
}
