/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <QObject>

#include <kulloclient/api/SyncPhase.h>

namespace ApiMirror {
namespace Enums {

class SyncPhases : public QObject
{
    Q_OBJECT

public:
    enum class SyncPhase
    {
        Keys,
        Profile,
        IncomingMessages,
        IncomingAttachments,
        OutgoingMessages,
    };
    Q_ENUM(SyncPhase)

    inline static void registerEnumsInClassForSignalSlot() {
        qRegisterMetaType<SyncPhases::SyncPhase>("SyncPhases::SyncPhase");
    }

    inline static SyncPhases::SyncPhase convert(Kullo::Api::SyncPhase value) {
        switch (value) {
        case Kullo::Api::SyncPhase::Keys:
            return SyncPhase::Keys;
        case Kullo::Api::SyncPhase::Profile:
            return SyncPhase::Profile;
        case Kullo::Api::SyncPhase::IncomingMessages:
            return SyncPhase::IncomingMessages;
        case Kullo::Api::SyncPhase::IncomingAttachments:
            return SyncPhase::IncomingAttachments;
        case Kullo::Api::SyncPhase::OutgoingMessages:
            return SyncPhase::OutgoingMessages;
        }
    }
};

}
}
