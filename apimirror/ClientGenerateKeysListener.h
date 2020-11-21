/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#pragma once

#include <QObject>

#include <kulloclient/api/ClientGenerateKeysListener.h>
#include <kulloclient/api/Registration.h>

#include "apimirror/misc.h"

namespace ApiMirror {

class ClientGenerateKeysListener
        : public QObject
        , public Kullo::Api::ClientGenerateKeysListener
{
    Q_OBJECT

public:
    explicit ClientGenerateKeysListener(QObject *parent = nullptr)
        : QObject(parent)
    {
        // no registration needed for: int
        K_REGISTER_QT_META_TYPE(std::shared_ptr<Kullo::Api::Registration>);
    }

    void progress(int8_t progress) override
    {
        emit _progress(progress);
    }

    void finished(const Kullo::nn_shared_ptr<Kullo::Api::Registration> &registration) override
    {
        emit _finished(registration.as_nullable());
    }

signals:
    void _progress(int progress);
    void _finished(const std::shared_ptr<Kullo::Api::Registration> &registration);
};

}
