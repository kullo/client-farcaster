/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#include "utils.h"

#include <QApplication>
#include <QClipboard>
#include <QFileInfo>

#include <desktoputil/stlqt.h>
#include <kulloclient/util/masterkey.h>

namespace KulloDesktop {
namespace Qml {

QStringList Utils::extractDataFromPemMasterKey(const QString &key) const
{
    auto masterKey = Kullo::Util::MasterKey(key.toStdString());
    return DesktopUtil::StlQt::toQStringList(masterKey.dataBlocks());
}

bool Utils::isValidPemMasterKey(const QString &key) const
{
    return Kullo::Util::MasterKey::isValid(key.toStdString());
}

void Utils::setClipboardText(QString txt) const
{
    QApplication::clipboard()->setText(txt);
}

QUrl Utils::makeFileUrl(const QString &path) const
{
    QString binaryPath = QFileInfo(QCoreApplication::applicationFilePath()).absolutePath();
    QString url("file://");
#if defined Q_OS_WIN
    url += "/" + binaryPath + "/" + path;
#elif defined Q_OS_OSX
    url += binaryPath + "/../Resources/" + path;
#elif defined Q_OS_LINUX
    url += binaryPath + "/" + path;
#endif
    return QUrl(url);
}

QColor Utils::setSaturation(const QColor &in, const qreal saturation) const
{
    QColor out;
    qreal inHue, inSat, inValue, inAlpha;
    in.getHsvF(&inHue, &inSat, &inValue, &inAlpha);
    out.setHsvF(inHue, saturation, inValue, inAlpha);
    return out;
}

QString Utils::urlencode(const QString &in) const
{
    return QString(QUrl::toPercentEncoding(in));
}

}
}
