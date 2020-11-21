/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#pragma once

#include <memory>

#include <kulloclient/api_impl/MasterKey.h>
#include <kulloclient/kulloclient-forwards.h>

class Registerer final
{
public:
    Kullo::Api::MasterKey run(const Kullo::Api::Address &address);
};
