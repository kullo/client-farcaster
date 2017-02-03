/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#include "loginchecker.h"

#include <QQmlEngine>
#include <QStringList>

#include <apimirror/Client.h>
#include <apimirror/ClientCheckCredentialsListener.h>
#include <desktoputil/stlqt.h>
#include <kulloclient/api/Client.h>
#include <kulloclient/api/MasterKey.h>
#include <kulloclient/api/NetworkError.h>
#include <kulloclient/util/assert.h>
#include <kulloclient/util/librarylogger.h>

namespace KulloDesktop {
namespace QmlComponents {

LoginChecker::LoginChecker(QObject *parent)
    : QObject(parent)
{
    listener_ = std::make_shared<ApiMirror::ClientCheckCredentialsListener>();

    connect(listener_.get(), &ApiMirror::ClientCheckCredentialsListener::_finished,
            this, [this](std::shared_ptr<Kullo::Api::Address> address, std::shared_ptr<Kullo::Api::MasterKey> masterKey, bool exists)
    {
        setLocked(false);
        emit loginChecked(QString::fromStdString(address->toString()),
                          QString::fromStdString(masterKey->pem()),
                          exists);
    });

    connect(listener_.get(), &ApiMirror::ClientCheckCredentialsListener::_error,
            this, [this](std::shared_ptr<Kullo::Api::Address> address, Kullo::Api::NetworkError error)
    {
        setLocked(false);
        switch (error) {
        case Kullo::Api::NetworkError::Server:
            Log.e() << "Server error.";
            emit serverError(QString::fromStdString(address->toString()));
            break;
        case Kullo::Api::NetworkError::Protocol:
            Log.e() << "Protocol error.";
            emit protocolError(QString::fromStdString(address->toString()));
            break;
        case Kullo::Api::NetworkError::Connection:
            Log.e() << "Connection error.";
            emit networkError(QString::fromStdString(address->toString()));
            break;
        default:
            kulloAssert(false);
        }
    });
}

LoginChecker::~LoginChecker()
{
}

ApiMirror::Client *LoginChecker::client()
{
    QQmlEngine::setObjectOwnership(client_, QQmlEngine::CppOwnership);
    return client_;
}

void LoginChecker::setClient(ApiMirror::Client *client)
{
    kulloAssert(!client_);
    kulloAssert(client);
    client_ = client;
    emit clientChanged();
}

bool LoginChecker::locked() const
{
    return locked_;
}

void LoginChecker::run(const QString &addr, const QStringList &masterKeyBlocks)
{
    if (locked_) return;

    setLocked(true);

    auto masterKeyBlocksStd = DesktopUtil::StlQt::toVector(masterKeyBlocks);

    auto address = Kullo::Api::Address::create(addr.toStdString());
    if (!address)
    {
        emit invalidKulloAddress();
        setLocked(false);
        return;
    }

    auto masterKey = Kullo::Api::MasterKey::createFromDataBlocks(masterKeyBlocksStd);
    if (!masterKey)
    {
        emit invalidMasterKey();
        setLocked(false);
        return;
    }

    bgTask_ = client_->raw()->checkCredentialsAsync(
                address,
                masterKey,
                listener_);
}

void LoginChecker::setLocked(bool locked)
{
    if (locked_ == locked) return;

    locked_ = locked;
    emit lockedChanged();
}

}
}
