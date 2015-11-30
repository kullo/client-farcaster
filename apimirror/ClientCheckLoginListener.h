/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#pragma once

#include <QObject>

#include <kulloclient/api/Address.h>
#include <kulloclient/api/ClientCheckLoginListener.h>

namespace ApiMirror {

class ClientCheckLoginListener
        : public QObject
        , public Kullo::Api::ClientCheckLoginListener
{
    Q_OBJECT

public:
    explicit ClientCheckLoginListener(QObject *parent = nullptr) : QObject(parent)
    {
        qRegisterMetaType<std::shared_ptr<Kullo::Api::Address>>("std::shared_ptr<Kullo::Api::Address>");
        qRegisterMetaType<std::shared_ptr<Kullo::Api::MasterKey>>("std::shared_ptr<Kullo::Api::MasterKey>");
        qRegisterMetaType<Kullo::Api::NetworkError>("Kullo::Api::NetworkError");
    }

    void finished(const std::shared_ptr<Kullo::Api::Address> & address, const std::shared_ptr<Kullo::Api::MasterKey> & masterKey, bool exists) override
    {
        emit _finished(address, masterKey, exists);
    }

    void error(const std::shared_ptr<Kullo::Api::Address> & address, Kullo::Api::NetworkError e) override
    {
        emit _error(address, e);
    }

signals:
    void _finished(const std::shared_ptr<Kullo::Api::Address> & address, const std::shared_ptr<Kullo::Api::MasterKey> & masterKey, bool exists);
    void _error(const std::shared_ptr<Kullo::Api::Address> & address, Kullo::Api::NetworkError error);
};

}
