/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#pragma once

#include <mutex>

namespace DesktopUtil {

class ThreadBlocker final
{
public:
    // the definition of success is defined by the caller. It is just passed through the interface
    void release(bool success);

    // returns the success value passed into release()
    bool block();

private:
    std::condition_variable cv_;
    std::mutex mutex_;
    bool otherThreadIsDone_ = false;
    bool otherThreadReportedSuccess_;
};

}
