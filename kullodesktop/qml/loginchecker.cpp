/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#include "loginchecker.h"

#include <QQmlEngine>
#include <QStringList>

#include <desktoputil/stlqt.h>

#include <apimirror/Client.h>
#include <apimirror/ClientCheckCredentialsListener.h>

#include <kulloclient/api/Client.h>
#include <kulloclient/api/MasterKey.h>
#include <kulloclient/api/NetworkError.h>

#include <kulloclient/util/assert.h>
#include <kulloclient/util/kulloaddress.h>
#include <kulloclient/util/librarylogger.h>
#include <kulloclient/util/masterkey.h>

namespace KulloDesktop {
namespace Qml {

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

    if (!Kullo::Util::KulloAddress::isValid(addr.toStdString()))
    {
        emit invalidKulloAddress();
        setLocked(false);
        return;
    }

    if (!Kullo::Util::MasterKey::isValid(masterKeyBlocksStd))
    {
        emit invalidMasterKey();
        setLocked(false);
        return;
    }

    bgTask_ = client_->raw()->checkCredentialsAsync(
                Kullo::Api::Address::create(addr.toStdString()),
                Kullo::Api::MasterKey::createFromDataBlocks(masterKeyBlocksStd),
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
