/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#include "existencechecker.h"

#include <QQmlEngine>

#include <apimirror/Client.h>
#include <apimirror/ClientAddressExistsListener.h>

#include <kulloclient/api/Address.h>
#include <kulloclient/api/Client.h>
#include <kulloclient/api/NetworkError.h>
#include <kulloclient/util/assert.h>
#include <kulloclient/util/misc.h>
#include <kulloclient/util/librarylogger.h>

namespace KulloDesktop {
namespace QmlComponents {

ExistenceChecker::ExistenceChecker(QObject *parent) :
    QObject(parent)
{
    listener_ = std::make_shared<ApiMirror::ClientAddressExistsListener>();

    connect(listener_.get(), &ApiMirror::ClientAddressExistsListener::_finished,
            this, &ExistenceChecker::onExistenceChecked);
    connect(listener_.get(), &ApiMirror::ClientAddressExistsListener::_error,
            this, &ExistenceChecker::onError);
}

ExistenceChecker::~ExistenceChecker()
{
}

ApiMirror::Client *ExistenceChecker::client()
{
    QQmlEngine::setObjectOwnership(client_, QQmlEngine::CppOwnership);
    return client_;
}

void ExistenceChecker::setClient(ApiMirror::Client *client)
{
    kulloAssert(!client_);
    kulloAssert(client);
    client_ = client;
    emit clientChanged();
}

bool ExistenceChecker::locked() const
{
    return locked_;
}

bool ExistenceChecker::checkExistence(const QString &addr)
{
    if (locked_) return false;
    setLocked(true);

    auto address = Kullo::Api::Address::create(addr.toStdString());
    if (!address)
    {
        Log.e() << "Existence check run with invalid Kullo address: "
                << addr.toStdString();
        emit invalidKulloAddress();
        setLocked(false);
        return true;
    }

    bgTask_ = client_->raw()->addressExistsAsync(address, listener_);
    return true;
}

void ExistenceChecker::onExistenceChecked(const std::string &address, bool ok)
{
    setLocked(false);

    emit existenceChecked(QString::fromStdString(address), ok);
}

void ExistenceChecker::onError(const std::string &address, Kullo::Api::NetworkError error)
{
    setLocked(false);

    switch (error) {
    case Kullo::Api::NetworkError::Server:
        Log.e() << "Server error.";
        emit serverError(QString::fromStdString(address));
        break;
    case Kullo::Api::NetworkError::Protocol:
        Log.e() << "Protocol error.";
        emit protocolError(QString::fromStdString(address));
        break;
    case Kullo::Api::NetworkError::Connection:
        Log.e() << "Connection error.";
        emit networkError(QString::fromStdString(address));
        break;
    default:
        kulloAssert(false);
    }
}

void ExistenceChecker::setLocked(bool locked)
{
    if (locked_ == locked) return;

    locked_ = locked;
    emit lockedChanged();
}

}
}
