/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#include "participantavatarprovider.h"

#include <QUrl>
#include <boost/optional.hpp>
#include <desktoputil/qtypestreamers.h>
#include <kulloclient/util/kulloaddress.h>
#include <kulloclient/util/librarylogger.h>

#include "kullodesktop/qml/clientmodel.h"

namespace KulloDesktop {
namespace Imageproviders {

ParticipantAvatarProvider::ParticipantAvatarProvider(Qml::ClientModel &clientModel)
    : AbstractAvatarProvider(clientModel)
{
}

QPixmap ParticipantAvatarProvider::drawAvatar(const QString &url, const QSize &renderSize)
{
    if (!clientModel_.loggedIn())
    {
        return QPixmap();
    }

    // Cut query string from URL
    QString path = url.split("?").at(0);

    boost::optional<Kullo::Util::KulloAddress> address;
    try {
        auto addressString = QUrl::fromPercentEncoding(path.toUtf8());;
        address = Kullo::Util::KulloAddress(addressString.toStdString());
    }
    catch (std::invalid_argument)
    {
        Log.e() << "Invalid Kullo address in avatar request string: " << url;
        return QPixmap();
    }

    auto participantsList = clientModel_.participantsModels();

    if (!participantsList.count(*address))
    {
        Log.d() << "Participant address not found: " << *address;
        return rounded(getEmptyAvatar(renderSize));
    }

    auto participant = participantsList[*address];
    return rounded(getParticipantAvatar(*participant, renderSize));
}

}
}
