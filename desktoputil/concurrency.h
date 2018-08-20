/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
#pragma once

#include <future>

namespace DesktopUtil {

bool waitOrCrash(std::promise<bool> &promise);

}
