/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <QObject>

#include <kulloclient/api/ClientCreateSessionListener.h>

namespace ApiMirror {

class ClientCreateSessionListener : public QObject, public Kullo::Api::ClientCreateSessionListener
{
    Q_OBJECT

public:
    explicit ClientCreateSessionListener(QObject *parent = nullptr)
        : QObject(parent)
    {
        qRegisterMetaType<std::shared_ptr<Kullo::Api::Session>>("std::shared_ptr<Kullo::Api::Session>");
        qRegisterMetaType<std::shared_ptr<Kullo::Api::Address>>("std::shared_ptr<Kullo::Api::Address>");
        qRegisterMetaType<Kullo::Api::LocalError>("Kullo::Api::LocalError");
    }

    void finished(const std::shared_ptr<Kullo::Api::Session> &session) override
    {
        emit _finished(session);
    }

    void error(const std::shared_ptr<Kullo::Api::Address> &address, Kullo::Api::LocalError error) override
    {
        emit _error(address, error);
    }

signals:
    void _finished(const std::shared_ptr<Kullo::Api::Session> &session);
    void _error(const std::shared_ptr<Kullo::Api::Address> &address, Kullo::Api::LocalError error);
};

}
