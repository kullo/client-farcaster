/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#include "singleinstancelock.h"

#include <cstdint>

#include <kulloclient/util/librarylogger.h>
#include <desktoputil/qtypestreamers.h>

namespace KulloDesktop {
namespace OsIntegration {

namespace {
const auto LOCKVALUE_SIZE = 1; // bytes
const auto LOCKVALUE_LOCKED = std::uint8_t{1};
const auto LOCKVALUE_UNLOCKED = std::uint8_t{0};
}

SingleInstanceLock::SingleInstanceLock(const QString &name, bool ignoreLockValue)
    : shared_(name)
    , ignoreLockValue_(ignoreLockValue)
{
}

bool SingleInstanceLock::acquire()
{
    if (!shared_.create(LOCKVALUE_SIZE, QSharedMemory::ReadWrite))
    {
        Log.d() << "Shared memory already exists.";
        if (!shared_.attach(QSharedMemory::ReadWrite))
        {
            const auto error = shared_.errorString();
            Log.f() << "Shared memory could not be attached: " << error;
        }
    }

    if (ignoreLockValue_)
    {
        // recover from crash
    }
    else
    {
        shared_.lock();
        const std::uint8_t *ret_ptr = static_cast<const std::uint8_t *>(shared_.constData());
        const std::uint8_t ret = *ret_ptr;
        shared_.unlock();

        if (ret == LOCKVALUE_LOCKED)
        {
            return false;
        }
    }

    shared_.lock();
    auto write_ptr = static_cast<std::uint8_t *>(shared_.data());
    *write_ptr = LOCKVALUE_LOCKED;
    shared_.unlock();

    return true;
}

void SingleInstanceLock::release()
{
    shared_.lock();
    auto write_ptr = static_cast<std::uint8_t *>(shared_.data());
    *write_ptr = LOCKVALUE_UNLOCKED;
    shared_.unlock();
}

}
}
