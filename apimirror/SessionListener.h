/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
#pragma once

#include <QObject>

#include <kulloclient/api/SessionListener.h>

#include "apimirror/misc.h"

namespace ApiMirror {

class SessionListener : public QObject, public Kullo::Api::SessionListener
{
    Q_OBJECT

public:
    explicit SessionListener(QObject *parent = nullptr)
        : QObject(parent)
    {
        K_REGISTER_QT_META_TYPE(std::shared_ptr<Kullo::Api::InternalEvent>);
    }

    void internalEvent(const Kullo::nn_shared_ptr<Kullo::Api::InternalEvent> &event) override
    {
        emit _internalEvent(event);
    }

signals:
    void _internalEvent(const std::shared_ptr<Kullo::Api::InternalEvent> &event);
};

}
