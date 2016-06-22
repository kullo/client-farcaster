/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <memory>
#include <string>

#include <kulloclient/kulloclient-forwards.h>

class Registerer final
{
public:
    std::shared_ptr<Kullo::Api::MasterKey> run(
            const std::shared_ptr<Kullo::Api::Address> &address);
};
