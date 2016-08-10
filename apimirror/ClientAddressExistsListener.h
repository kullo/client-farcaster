/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <QObject>

#include <kulloclient/api/Address.h>
#include <kulloclient/api/ClientAddressExistsListener.h>

namespace ApiMirror {

class ClientAddressExistsListener
        : public QObject
        , public Kullo::Api::ClientAddressExistsListener
{
    Q_OBJECT

public:
    explicit ClientAddressExistsListener(QObject *parent = nullptr)
        : QObject(parent)
    {
        // no registration needded for: bool
        // registered in registerMetaTypes(): std::string
        qRegisterMetaType<Kullo::Api::NetworkError>("Kullo::Api::NetworkError");
    }

    void finished(const std::shared_ptr<Kullo::Api::Address> &address, bool exists) override
    {
        emit _finished(address->toString(), exists);
    }

    void error(const std::shared_ptr<Kullo::Api::Address> &address, Kullo::Api::NetworkError e) override
    {
        emit _error(address->toString(), e);
    }

signals:
    void _finished(std::string address, bool exists);
    void _error(std::string address, Kullo::Api::NetworkError error);
};

}
