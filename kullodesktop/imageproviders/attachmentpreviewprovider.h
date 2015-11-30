/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#pragma once

#include <QQuickImageProvider>

#include "kullodesktop/farcaster-forwards.h"

namespace KulloDesktop {
namespace Imageproviders {

class AttachmentPreviewProvider : public QQuickImageProvider
{
public:
    explicit AttachmentPreviewProvider(Qml::ClientModel &clientModel);
    QPixmap requestPixmap(const QString &url, QSize *size, const QSize &requestedSize);

private:
    Qml::ClientModel &clientModel_;
};

}
}
