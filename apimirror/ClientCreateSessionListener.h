/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#pragma once

#include <QObject>

#include <kulloclient/api/ClientCreateSessionListener.h>
#include <kulloclient/api_impl/Address.h>

#include "apimirror/misc.h"
#include "apimirror/signalslotvalue.h"

namespace ApiMirror {

class ClientCreateSessionListener : public QObject, public Kullo::Api::ClientCreateSessionListener
{
    Q_OBJECT

public:
    explicit ClientCreateSessionListener(QObject *parent = nullptr)
        : QObject(parent)
    {
        K_REGISTER_QT_META_TYPE(std::shared_ptr<Kullo::Api::Session>);
        K_REGISTER_QT_META_TYPE(SignalSlotValue<Kullo::Api::Address>);
        K_REGISTER_QT_META_TYPE(Kullo::Api::LocalError);
    }

    void migrationStarted(const Kullo::Api::Address &address) override
    {
        emit _migrationStarted(address);
    }

    void finished(const Kullo::nn_shared_ptr<Kullo::Api::Session> &session) override
    {
        emit _finished(session);
    }

    void error(const Kullo::Api::Address &address, Kullo::Api::LocalError error) override
    {
        emit _error(address, error);
    }

signals:
    void _migrationStarted(const SignalSlotValue<Kullo::Api::Address> &address);
    void _finished(const std::shared_ptr<Kullo::Api::Session> &session);
    void _error(const SignalSlotValue<Kullo::Api::Address> &address, Kullo::Api::LocalError error);
};

}
