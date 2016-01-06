/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <QObject>

#include <kulloclient/api/ClientGenerateKeysListener.h>
#include <kulloclient/api/Registration.h>

namespace ApiMirror {

class ClientGenerateKeysListener
        : public QObject
        , public Kullo::Api::ClientGenerateKeysListener
{
    Q_OBJECT

public:
    explicit ClientGenerateKeysListener(QObject *parent = nullptr) : QObject(parent)
    {
        qRegisterMetaType<std::shared_ptr<Kullo::Api::Registration>>("std::shared_ptr<Kullo::Api::Registration>");
    }

    void progress(int8_t progress) override
    {
        emit _progress(progress);
    }

    void finished(const std::shared_ptr<Kullo::Api::Registration> & registration) override
    {
        emit _finished(registration);
    }

signals:
    void _progress(int progress);
    void _finished(const std::shared_ptr<Kullo::Api::Registration> & registration);
};

}
