/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
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
