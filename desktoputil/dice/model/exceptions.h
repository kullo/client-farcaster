/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <kulloclient/util/exceptions.h>

namespace Kullo {
namespace Model {

/**
 * @brief An exception that is thrown if something cannot be found in DB.
 */
class NotFound : public Util::BaseException
{
public:
    /// @copydoc BaseException::BaseException(const std::string&)
    NotFound(const std::string &message = "") throw() : BaseException(message) {}
    virtual ~NotFound() = default;
};

}
}
