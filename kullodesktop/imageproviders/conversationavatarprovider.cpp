/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#include "conversationavatarprovider.h"

#include <QPainter>
#include <desktoputil/qtypestreamers.h>
#include <kulloclient/util/assert.h>
#include <kulloclient/util/librarylogger.h>

#include "kullodesktop/qml/clientmodel.h"
#include "kullodesktop/qml/conversationlistmodel.h"
#include "kullodesktop/qml/conversationlistsource.h"
#include "kullodesktop/qml/conversationmodel.h"
#include "kullodesktop/qml/participantmodel.h"

namespace KulloDesktop {
namespace Imageproviders {

ConversationAvatarProvider::ConversationAvatarProvider(Qml::ClientModel &clientModel) :
    AbstractAvatarProvider(clientModel)
{
}

QPixmap ConversationAvatarProvider::drawAvatar(const QString &url, const QSize &renderSize)
{
    if (!clientModel_.loggedIn())
    {
        return QPixmap();
    }

    // Cut query string from URL
    QString path = url.split("?").at(0);

    bool okConvId;
    quint32 convId = path.toUInt(&okConvId);
    if (!okConvId || convId == 0)
    {
        Log.e() << "Invalid conversation avatar request string: " << url;
        return QPixmap();
    }

    Qml::ConversationModel *conv = clientModel_.conversationsListSource()->get(convId);
    if (!conv)
    {
        Log.e() << "Conversation for conversation avatar not found: " << convId;
        return QPixmap();
    }

    auto participants = conv->participantsModels();

    QPixmap out;
    if (participants.size() == 1)
    {
        out = getParticipantAvatar(*participants[0], renderSize);
    }
    else if (participants.size() <= 4)
    {
        out = composeAvatar(participants, renderSize);
    }
    else
    {
        out = getFallbackAvatar(QString::number(participants.size()), renderSize);
    }

    return rounded(out);
}

QPixmap ConversationAvatarProvider::composeAvatar(const std::vector<std::unique_ptr<Qml::ParticipantModel> > &participants, const QSize &renderSize)
{
    const auto len = participants.size();
    kulloAssert(len >= 2 && len <= 4);

    QSize halfRenderSize = renderSize / 2;
    QPixmap out(renderSize);
    out.fill(Qt::transparent);
    QPainter painter(&out);

    if (len == 2)
    {
        //   |
        // 1 | 2
        //   |
        QPixmap pm1 = getParticipantAvatar(*participants[0], renderSize);
        QPixmap pm2 = getParticipantAvatar(*participants[1], renderSize);
        painter.drawPixmap(                   0, 0, pm1, pm1.width()/4, 0, pm1.width()/2-1, pm1.height());
        painter.drawPixmap(renderSize.width()/2, 0, pm2, pm2.width()/4, 0, pm2.width()/2, pm2.height());
    }
    else if (len == 3)
    {
        //   | 2
        // 1 |--
        //   | 3
        QPixmap pm1 = getParticipantAvatar(*participants[0], renderSize);
        QPixmap pm2 = getParticipantAvatar(*participants[1], halfRenderSize);
        QPixmap pm3 = getParticipantAvatar(*participants[2], halfRenderSize);
        painter.drawPixmap(                   0,                     0, pm1, pm1.width()/4, 0, pm1.width()/2-1, pm1.height());
        painter.drawPixmap(renderSize.width()/2,                     0, pm2, 0, 0, pm2.width(), pm2.height()-1);
        painter.drawPixmap(renderSize.width()/2, renderSize.height()/2, pm3, 0, 0, pm3.width(), pm3.height());
    }
    else if (len == 4)
    {
        // 1 | 2
        // -----
        // 3 | 4
        QPixmap pm1 = getParticipantAvatar(*participants[0], halfRenderSize);
        QPixmap pm2 = getParticipantAvatar(*participants[1], halfRenderSize);
        QPixmap pm3 = getParticipantAvatar(*participants[2], halfRenderSize);
        QPixmap pm4 = getParticipantAvatar(*participants[3], halfRenderSize);
        painter.drawPixmap(                   0,                     0, pm1, 0, 0, pm1.width()-1, pm1.height()-1);
        painter.drawPixmap(renderSize.width()/2,                     0, pm2, 0, 0, pm2.width(),   pm2.height()-1);
        painter.drawPixmap(                   0, renderSize.height()/2, pm3, 0, 0, pm3.width()-1, pm3.height());
        painter.drawPixmap(renderSize.width()/2, renderSize.height()/2, pm4, 0, 0, pm4.width(),   pm4.height());
    }

    return out;
}

}
}
