/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
#pragma once

#include <memory>

#include <kulloclient/api_impl/MasterKey.h>
#include <kulloclient/kulloclient-forwards.h>

class Registerer final
{
public:
    Kullo::Api::MasterKey run(const Kullo::Api::Address &address);
};
