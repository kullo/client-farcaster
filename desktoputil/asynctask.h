/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <functional>
#include <thread>

namespace DesktopUtil {

class AsyncTask
{
public:
    /// Run function in background and crash hard when exception is caught
    static void runDetached(std::function<void()> func);
    static std::thread run(std::function<void()> func);
};

}
