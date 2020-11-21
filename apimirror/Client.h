/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#pragma once

#include <memory>
#include <QObject>

#include <kulloclient/kulloclient-forwards.h>

namespace ApiMirror {

class Client : public QObject
{
    Q_OBJECT

public:
    explicit Client(QObject *parent = nullptr);
    explicit Client(std::shared_ptr<Kullo::Api::Client> client);

    std::shared_ptr<Kullo::Api::Client> raw() const;

private:
    std::shared_ptr<Kullo::Api::Client> client_;
};

}
