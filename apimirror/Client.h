/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
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
