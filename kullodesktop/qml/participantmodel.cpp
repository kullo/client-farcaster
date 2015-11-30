/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#include "participantmodel.h"

#include <desktoputil/stlqt.h>
#include <desktoputil/dice/model/participant.h>
#include <kulloclient/util/librarylogger.h>

namespace KulloDesktop {
namespace Qml {

ParticipantModel::ParticipantModel(QObject *parent)
    : QObject(parent)
{
    Log.e() << "Don't instantiate Participant in QML.";
}

ParticipantModel::ParticipantModel(std::shared_ptr<Kullo::Model::Participant> part, QObject *parent)
    : QObject(parent)
    , part_(part)
{
}

QString ParticipantModel::name() const
{
    return QString::fromStdString(part_->name());
}

QString ParticipantModel::address() const
{
    return QString::fromStdString(part_->address().toString());
}

QString ParticipantModel::organization() const
{
    return QString::fromStdString(part_->organization());
}

QPixmap ParticipantModel::avatar() const
{
    QByteArray type;
    if (part_->avatarMimeType() == "image/png")  type = "png";
    if (part_->avatarMimeType() == "image/jpeg") type = "jpeg";

    QPixmap out;
    QByteArray avatarData = DesktopUtil::StlQt::toQByteArray(part_->avatar());
    if (out.loadFromData(avatarData, type.constData()))
    {
        return out;
    }
    return QPixmap();
}

}
}
