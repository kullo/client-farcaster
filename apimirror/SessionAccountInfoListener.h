/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#pragma once

#include <QObject>

#include <kulloclient/api/AccountInfo.h>
#include <kulloclient/api/SessionAccountInfoListener.h>

#include "apimirror/misc.h"
#include "apimirror/signalslotvalue.h"

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
        K_REGISTER_QT_META_TYPE(SignalSlotValue<Kullo::Api::AccountInfo>);
    }

    void finished(const Kullo::Api::AccountInfo &accountInfo) override
    {
        emit _finished(accountInfo);
    }

    void error(Kullo::Api::NetworkError error) override
    {
        emit _error(error);
    }

signals:
    void _finished(const SignalSlotValue<Kullo::Api::AccountInfo> &accountInfo);
    void _error(Kullo::Api::NetworkError error);
};

}
