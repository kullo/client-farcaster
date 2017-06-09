/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#pragma once

#include <QObject>

#include <kulloclient/api/ClientCreateSessionListener.h>

#include "apimirror/misc.h"

namespace ApiMirror {

class ClientCreateSessionListener : public QObject, public Kullo::Api::ClientCreateSessionListener
{
    Q_OBJECT

public:
    explicit ClientCreateSessionListener(QObject *parent = nullptr)
        : QObject(parent)
    {
        K_REGISTER_QT_META_TYPE(std::shared_ptr<Kullo::Api::Session>);
        K_REGISTER_QT_META_TYPE(std::shared_ptr<Kullo::Api::Address>);
        K_REGISTER_QT_META_TYPE(Kullo::Api::LocalError);
    }

    void migrationStarted(const std::shared_ptr<Kullo::Api::Address> &address) override
    {
        emit _migrationStarted(address);
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
    void _migrationStarted(const std::shared_ptr<Kullo::Api::Address> &address);
    void _finished(const std::shared_ptr<Kullo::Api::Session> &session);
    void _error(const std::shared_ptr<Kullo::Api::Address> &address, Kullo::Api::LocalError error);
};

}
