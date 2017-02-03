/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#pragma once

#include <memory>
#include <QObject>

#include <apimirror/apimirror-forwards.h>
#include <kullodesktop/farcaster-forwards.h>
#include <kulloclient/kulloclient-forwards.h>

namespace KulloDesktop {
namespace QmlComponents {

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

    Q_PROPERTY(KulloDesktop::Qml::InnerApplication* application READ application WRITE setApplication NOTIFY applicationChanged)
    Qml::InnerApplication *application() const;
    void setApplication(Qml::InnerApplication *application);

    Q_PROPERTY(ApiMirror::Client* client READ client WRITE setClient NOTIFY clientChanged)
    ApiMirror::Client *client() const;
    void setClient(ApiMirror::Client *client);

    Q_PROPERTY(bool locked READ locked WRITE setLocked NOTIFY lockedChanged)
    bool locked() const;
    void setLocked(bool locked);

    Q_PROPERTY(QString termsUrl READ termsUrl NOTIFY termsUrlChanged)
    QString termsUrl() const;

    Q_INVOKABLE void generateKeys();
    Q_INVOKABLE void registerAccount(const QString &addr, const QString &challengeAnswer);

signals:
    void applicationChanged();
    void clientChanged();
    void lockedChanged();
    void termsUrlChanged();
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
    Qml::InnerApplication *application_ = nullptr;
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
