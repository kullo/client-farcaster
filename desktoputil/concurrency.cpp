/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
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
