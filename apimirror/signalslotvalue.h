/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#pragma once

#include <stdexcept>
#include <boost/optional.hpp>

// Wrapper of Type with a dummy default constructor that compiles but must not be used
// Note: This must always be used with absolute namespaces to ensure signal/slot
//       string based magick works. We use the global namespace to avoid any kind
//       of runtime suprises that pass compiler.

template <class Type>
class SignalSlotValue final
{
public:
    explicit SignalSlotValue() {
        throw std::logic_error("Constructor only exists due to Qt signal/slot requirements "
                               "and has no valid semantics. Do not use it.");
    }

    SignalSlotValue(const Type &data)
        : container_(data)
    {
    }

    SignalSlotValue(const SignalSlotValue &other)
        : container_(other.container_)
    {
    }

    const Type & operator*() const {
        return container_.get();
    }

    const Type * operator->() const {
        return container_.get_ptr();
    }

private:
    const boost::optional<Type> container_;
};
