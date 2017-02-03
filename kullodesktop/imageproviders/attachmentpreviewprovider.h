/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#pragma once

#include <QQuickImageProvider>

#include "kullodesktop/farcaster-forwards.h"

namespace KulloDesktop {
namespace Imageproviders {

class AttachmentPreviewProvider : public QQuickImageProvider
{
public:
    explicit AttachmentPreviewProvider(Qml::Inbox &inbox);
    QPixmap requestPixmap(const QString &url, QSize *size, const QSize &requestedSize);

private:
    Qml::Inbox &inbox_;
};

}
}
