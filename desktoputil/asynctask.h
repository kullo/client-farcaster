/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
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
