/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#pragma once

#include <memory>

#include <QObject>

#include <kulloclient/api/AccountInfo.h>
#include <kulloclient/api/SessionAccountInfoListener.h>

#define K_REGISTER_QT_META_TYPE(t) qRegisterMetaType<t>(#t)

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
        K_REGISTER_QT_META_TYPE(Kullo::Api::NetworkError);

        // wrap in shared_ptr to make it Qt meta type compatible
        // (i.e. public default constructor + public copy constructor + public destructor)
        K_REGISTER_QT_META_TYPE(std::shared_ptr<Kullo::Api::AccountInfo>);
    }

    void finished(const Kullo::Api::AccountInfo &accountInfo) override
    {
        emit _finished(std::make_shared<Kullo::Api::AccountInfo>(accountInfo));
    }

    void error(Kullo::Api::NetworkError error) override
    {
        emit _error(error);
    }

signals:
    void _finished(const std::shared_ptr<Kullo::Api::AccountInfo> accountInfo);
    void _error(Kullo::Api::NetworkError error);
};

}
