/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#pragma once

#include <QObject>

#include <kulloclient/api/SyncPhase.h>

namespace ApiMirror {
namespace Enums {

class SyncPhaseHolder : public QObject
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
        qRegisterMetaType<SyncPhaseHolder::SyncPhase>("SyncPhaseHolder::SyncPhase");
    }

    inline static SyncPhaseHolder::SyncPhase convert(Kullo::Api::SyncPhase value) {
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
