/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#pragma once

#include <memory>

#include "kullodesktop/imageproviders/abstractavatarprovider.h"

namespace KulloDesktop {
namespace Imageproviders {

class ConversationAvatarProvider : public AbstractAvatarProvider
{
public:
    ConversationAvatarProvider(Qml::ClientModel &clientModel);

protected:
    virtual QPixmap drawAvatar(const QString &url, const QSize &renderSize);

private:
    QPixmap composeAvatar(const std::vector<std::unique_ptr<Qml::ParticipantModel>> &participants, const QSize &renderSize);
};

}
}
