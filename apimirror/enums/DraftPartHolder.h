/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <QObject>

#include <kulloclient/api/DraftPart.h>

namespace ApiMirror {
namespace Enums {

class DraftPartHolder : public QObject
{
    Q_OBJECT

public:
    enum DraftPart
    {
        Content,
        Attachments
    };
    Q_ENUM(DraftPart)

    inline static void registerEnumsInClassForSignalSlot()
    {
        qRegisterMetaType<DraftPartHolder::DraftPart>("DraftPartHolder::DraftPart");
    }

    inline static DraftPartHolder::DraftPart convert(Kullo::Api::DraftPart value)
    {
        switch (value) {
        case Kullo::Api::DraftPart::Content:
            return DraftPartHolder::Content;
        case Kullo::Api::DraftPart::Attachments:
            return DraftPartHolder::Attachments;
        }
    }
};

}
}
