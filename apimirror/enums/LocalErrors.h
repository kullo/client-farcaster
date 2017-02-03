/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <QObject>

#include <kulloclient/api/LocalError.h>

namespace ApiMirror {
namespace Enums {

// This is used as Class.EnumValue in QML (e.g. SyncErrors.Unauthorized),
// so the enum name is skipped.
class LocalErrors : public QObject
{
    Q_OBJECT

public:
    enum class LocalError
    {
        FileTooBig,
        Filesystem,
        Unknown,
    };
    Q_ENUM(LocalError)

    inline static void registerEnumsInClassForSignalSlot() {
        qRegisterMetaType<LocalErrors::LocalError>("LocalErrors::LocalError");
    }

    inline static LocalErrors::LocalError convert(Kullo::Api::LocalError value)
    {
        switch (value) {
        case Kullo::Api::LocalError::FileTooBig:
            return LocalError::FileTooBig;
        case Kullo::Api::LocalError::Filesystem:
            return LocalError::Filesystem;
        case Kullo::Api::LocalError::Unknown:
            return LocalError::Unknown;
        }
    }
};

}
}
