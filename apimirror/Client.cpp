/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
#include "Client.h"

#include <kulloclient/api/Client.h>

#include <kulloclient/util/assert.h>

namespace ApiMirror {

Client::Client(QObject *parent) : QObject(parent)
{
    kulloAssert(false);
}

Client::Client(std::shared_ptr<Kullo::Api::Client> client)
    : QObject(nullptr)
    , client_(client)
{
}

std::shared_ptr<Kullo::Api::Client> Client::raw() const
{
    auto ptr = client_;
    kulloAssert(ptr);
    return ptr;
}

}
