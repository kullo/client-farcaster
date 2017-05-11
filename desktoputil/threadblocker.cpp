#include "threadblocker.h"

#include <kulloclient/util/assert.h>
#include <kulloclient/util/misc.h>

namespace DesktopUtil {

namespace {
const auto threadBlockTimeout = std::chrono::seconds(
            #ifdef NDEBUG
            15
            #else
            1000
            #endif
            );
}

void ThreadBlocker::release(bool success)
{
    {
        std::lock_guard<std::mutex> lock(mutex_); K_UNUSED(lock);
        otherThreadIsDone_ = true;
        otherThreadReportedSuccess_ = success;
    }
    cv_.notify_one();
}

bool ThreadBlocker::block()
{
    std::unique_lock<std::mutex> lock(mutex_); // locks mutex

    // periodically releases and reacquires lock
    bool ok = cv_.wait_for(lock, threadBlockTimeout, [&]{ return otherThreadIsDone_; });
    kulloAssert(ok);

    bool out = otherThreadReportedSuccess_;

    lock.unlock();

    return out;
}

}
