/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#include "registerer.h"

#include <QQmlEngine>

#include <apimirror/Client.h>
#include <apimirror/ClientGenerateKeysListener.h>
#include <apimirror/RegistrationRegisterAccountListener.h>
#include <desktoputil/qtypestreamers.h>
#include <kulloclient/api/AddressHelpers.h>
#include <kulloclient/api/AddressNotAvailableReason.h>
#include <kulloclient/api/Challenge.h>
#include <kulloclient/api/ChallengeType.h>
#include <kulloclient/api/Client.h>
#include <kulloclient/api/AsyncTask.h>
#include <kulloclient/api/MasterKeyHelpers.h>
#include <kulloclient/api/NetworkError.h>
#include <kulloclient/api/Registration.h>
#include <kulloclient/api_impl/Address.h>
#include <kulloclient/api_impl/MasterKey.h>
#include <kulloclient/util/assert.h>
#include <kulloclient/util/librarylogger.h>
#include <kulloclient/util/misc.h>

#include "kullodesktop/qml/innerapplication.h"
#include "kullodesktop/qml/usersettings.h"

namespace KulloDesktop {
namespace QmlComponents {

namespace {
const QString TERMS_URL = QStringLiteral("https://www.kullo.net/agb/?version=1");
}

void ChallengeTypes::init()
{
    qRegisterMetaType<ChallengeTypes::ChallengeType>("ChallengeTypes::ChallengeType");
}

Registerer::Registerer(QObject *parent)
    : QObject(parent)
    , listenerKeygen_(Kullo::nn_make_shared<ApiMirror::ClientGenerateKeysListener>())
    , listenerRegistration_(Kullo::nn_make_shared<ApiMirror::RegistrationRegisterAccountListener>())
{
    ChallengeTypes::init();

    connect(listenerKeygen_.get(), &ApiMirror::ClientGenerateKeysListener::_progress,
            this, &Registerer::keysGenerationProgressChanged);
    connect(listenerKeygen_.get(), &ApiMirror::ClientGenerateKeysListener::_finished,
            this, [this](const std::shared_ptr<Kullo::Api::Registration> & registration)
    {
        registration_ = registration;
    });

    connect(listenerRegistration_.get(), &ApiMirror::RegistrationRegisterAccountListener::_addressNotAvailable,
            this, [this](const ApiMirror::SignalSlotValue<Kullo::Api::Address> &address, Kullo::Api::AddressNotAvailableReason reason)
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
            this, [this](const ApiMirror::SignalSlotValue<Kullo::Api::Address> &address, const std::shared_ptr<Kullo::Api::Challenge> & challenge)
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
            this, [this](const ApiMirror::SignalSlotValue<Kullo::Api::Address> &address, const ApiMirror::SignalSlotValue<Kullo::Api::MasterKey> &masterKey)
    {
        application_->databaseFiles().removeDatabase(*address);
        Qml::UserSettings::storeCredentials(*address, *masterKey);

        auto addressString = QString::fromStdString(address->toString());
        auto masterKeyString = QString::fromStdString(Kullo::Api::MasterKeyHelpers::toPem(*masterKey));
        emit succeeded(addressString, masterKeyString);
    });

    connect(listenerRegistration_.get(), &ApiMirror::RegistrationRegisterAccountListener::_error,
            this, [this](const ApiMirror::SignalSlotValue<Kullo::Api::Address> &address, Kullo::Api::NetworkError error)
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

Qml::InnerApplication *Registerer::application() const
{
    auto out = application_;
    QQmlEngine::setObjectOwnership(out, QQmlEngine::CppOwnership);
    return out;
}

void Registerer::setApplication(Qml::InnerApplication *application)
{
    kulloAssert(!application_);
    kulloAssert(application);
    application_ = application;
    emit applicationChanged();
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

QString Registerer::termsUrl() const
{
    return TERMS_URL;
}

void Registerer::generateKeys()
{
    // generateKeys() must be invoked only once per RegistererModel instance
    kulloAssert(!keyGenerationStarted_);
    keyGenerationStarted_ = true;

    taskKeygen_ = client_->raw()->generateKeysAsync(listenerKeygen_).as_nullable();
}

void Registerer::registerAccount(const QString &addr, const QString &challengeAnswer)
{
    kulloAssert(registration_);

    const std::string addressStr = addr.trimmed().toStdString();
    const std::string answer = challengeAnswer.trimmed().toStdString();

    Log.i() << "Trying to register " << addressStr << " ...";

    auto address = Kullo::Api::AddressHelpers::create(addressStr);
    if (address)
    {
        if (!answer.empty() && !lastChallenge_)
        {
            Log.f() << "Got challenge answer from user but no challenge is stored.";
        }

        if (lastChallenge_)
        {
            Log.i() << "With challenge: " << lastChallenge_->text();
        }
        else
        {
            Log.i() << "No challenge used.";
        }
        taskRegister_ = registration_->registerAccountAsync(
                    *address,
                    TERMS_URL.toStdString(),
                    lastChallenge_,
                    answer,
                    listenerRegistration_
                    ).as_nullable();
    }
    else
    {
        emit invalidKulloAddress(QString::fromStdString(addressStr));
    }
}

}
}
