/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#pragma once

#include <QObject>

#include <kulloclient/api/LocalError.h>

namespace ApiMirror {
namespace Enums {

// This is used as Class.EnumValue in QML (e.g. SyncErrors.Unauthorized),
// so the enum name is skipped.
class LocalErrorHolder : public QObject
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
        qRegisterMetaType<LocalErrorHolder::LocalError>("LocalErrorHolder::LocalError");
    }

    inline static LocalErrorHolder::LocalError convert(Kullo::Api::LocalError value)
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
