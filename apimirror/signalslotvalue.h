#pragma once

#include <stdexcept>
#include <boost/optional.hpp>

namespace ApiMirror {

// Wrapper of Type with a dummy default constructor that compiles but must not be used

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

}
