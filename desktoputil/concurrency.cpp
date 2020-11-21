/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#include "concurrency.h"

#include <kulloclient/util/assert.h>

namespace DesktopUtil {

namespace {
const auto maxBlockingPeriod = std::chrono::seconds(
            #ifdef NDEBUG
            15
            #else
            1000
            #endif
            );
}

bool waitOrCrash(std::promise<bool> &promise)
{
    auto successFuture = promise.get_future();
    auto status = successFuture.wait_for(maxBlockingPeriod);
    kulloAssert(status == std::future_status::ready);
    return successFuture.get();
}

}
