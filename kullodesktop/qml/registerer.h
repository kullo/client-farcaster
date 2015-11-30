/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#pragma once

#include <memory>
#include <QObject>

#include <apimirror/apimirror-forwards.h>
#include <kulloclient/kulloclient-forwards.h>

namespace Kullo {
namespace Api {
class Challenge; // Missing in libkullo v38
}
}

namespace KulloDesktop {
namespace Qml {

class ChallengeTypes : public QObject
{
    Q_OBJECT

public:
    enum ChallengeType
    {
        None = 0,
        Code,
        Reservation,
        Reset
    };
    Q_ENUMS(ChallengeType)

    static void init();
};

class Registerer : public QObject
{
    Q_OBJECT

public:
    explicit Registerer(QObject *parent = 0);
    ~Registerer();

    Q_PROPERTY(ApiMirror::Client* client READ client WRITE setClient NOTIFY clientChanged)
    ApiMirror::Client *client() const;
    void setClient(ApiMirror::Client *client);

    Q_PROPERTY(bool locked READ locked WRITE setLocked NOTIFY lockedChanged)
    bool locked() const;
    void setLocked(bool locked);

    Q_INVOKABLE void generateKeys();
    Q_INVOKABLE void registerAccount(const QString &addr, const QString &challengeAnswer);

signals:
    void clientChanged();
    void lockedChanged();
    void keysGenerationProgressChanged(int percent);
    void invalidKulloAddress(const QString &address);
    void addressExists(const QString &address);
    void addressBlocked(const QString &address);
    void challengeNeeded(ChallengeTypes::ChallengeType type, const QString &address, const QString &text);
    void succeeded(const QString &address, const QString &masterKeyPem);
    void serverError();
    void protocolError();
    void networkError();

private:
    ApiMirror::Client *client_ = nullptr;
    std::shared_ptr<ApiMirror::ClientGenerateKeysListener> listenerKeygen_;
    std::shared_ptr<ApiMirror::RegistrationRegisterAccountListener> listenerRegistration_;
    std::shared_ptr<Kullo::Api::Registration> registration_;
    std::shared_ptr<Kullo::Api::AsyncTask> taskKeygen_;
    std::shared_ptr<Kullo::Api::AsyncTask> taskRegister_;
    std::shared_ptr<Kullo::Api::Challenge> lastChallenge_;
    bool locked_ = false;
    bool keyGenerationStarted_ = false;
};

}
}
