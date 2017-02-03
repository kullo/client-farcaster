/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <QObject>

#include <kulloclient/api/NetworkError.h>

namespace ApiMirror {
namespace Enums {

// This is used as Class.EnumValue in QML (e.g. SyncErrors.Unauthorized),
// so the enum name is skipped.
class NetworkErrorHolder : public QObject
{
    Q_OBJECT

public:
    enum class NetworkError
    {
        Forbidden,
        Protocol,
        Unauthorized,
        Server,
        Connection,
        Unknown,
    };
    Q_ENUM(NetworkError)

    inline static void registerEnumsInClassForSignalSlot() {
        qRegisterMetaType<NetworkErrorHolder::NetworkError>("NetworkErrorHolder::NetworkError");
    }

    inline static NetworkErrorHolder::NetworkError convert(Kullo::Api::NetworkError value)
    {
        switch (value) {
        case Kullo::Api::NetworkError::Forbidden:
            return NetworkError::Forbidden;
        case Kullo::Api::NetworkError::Protocol:
            return NetworkError::Protocol;
        case Kullo::Api::NetworkError::Unauthorized:
            return NetworkError::Unauthorized;
        case Kullo::Api::NetworkError::Server:
            return NetworkError::Server;
        case Kullo::Api::NetworkError::Connection:
            return NetworkError::Connection;
        case Kullo::Api::NetworkError::Unknown:
            return NetworkError::Unknown;
        }
    }
};

}
}
