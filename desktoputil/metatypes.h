/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#pragma once

#include <exception>
#include <QMetaType>

#include <kulloclient/types.h>

Q_DECLARE_METATYPE(std::exception_ptr)

namespace DesktopUtil {

inline void registerMetaTypes()
{
    // Kullo types
    qRegisterMetaType<Kullo::id_type>("Kullo::id_type");
    qRegisterMetaType<Kullo::lastModified_type>("Kullo::lastModified_type");
    qRegisterMetaType<Kullo::timestamp_type>("Kullo::timestamp_type");
    qRegisterMetaType<Kullo::metaVersion_type>("Kullo::metaVersion_type");

    // libc types
    qRegisterMetaType<int64_t>("int64_t");

    // std types
    qRegisterMetaType<std::exception_ptr>("std::exception_ptr");
    qRegisterMetaType<std::size_t>("std::size_t");
    qRegisterMetaType<std::string>("std::string");
}

}
