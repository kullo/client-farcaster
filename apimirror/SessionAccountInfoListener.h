/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <QObject>

#include <kulloclient/api/AccountInfo.h>
#include <kulloclient/api/SessionAccountInfoListener.h>

namespace ApiMirror {

class SessionAccountInfoListener
        : public QObject
        , public Kullo::Api::SessionAccountInfoListener
{
    Q_OBJECT

public:
    explicit SessionAccountInfoListener(QObject *parent = nullptr)
        : QObject(parent)
    {
        // registered in registerMetaTypes(): std::string
        qRegisterMetaType<Kullo::Api::NetworkError>("Kullo::Api::NetworkError");
    }

    void finished(const Kullo::Api::AccountInfo &accountInfo) override
    {
        emit _finished(accountInfo.settingsUrl);
    }

    void error(Kullo::Api::NetworkError error) override
    {
        emit _error(error);
    }

signals:
    void _finished(const std::string &settingsUrl);
    void _error(Kullo::Api::NetworkError error);
};

}
