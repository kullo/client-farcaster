/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <exception>
#include <QMetaType>

#include <kulloclient/types.h>
#include <kulloclient/sync/definitions.h>

Q_DECLARE_METATYPE(Kullo::Sync::SyncMode)
Q_DECLARE_METATYPE(std::exception_ptr)

namespace DesktopUtil {

inline void registerMetaTypes()
{
    // Kullo types
    qRegisterMetaType<Kullo::id_type>("Kullo::id_type");
    qRegisterMetaType<Kullo::lastModified_type>("Kullo::lastModified_type");
    qRegisterMetaType<Kullo::timestamp_type>("Kullo::timestamp_type");
    qRegisterMetaType<Kullo::metaVersion_type>("Kullo::metaVersion_type");
    qRegisterMetaType<Kullo::Sync::SyncMode>("Kullo::Sync::SyncMode");
    qRegisterMetaType<Kullo::Sync::SyncProgress>("Kullo::Sync::SyncProgress");

    // std types
    qRegisterMetaType<std::exception_ptr>("std::exception_ptr");
    qRegisterMetaType<std::size_t>("std::size_t");
    qRegisterMetaType<std::string>("std::string");
}

}
