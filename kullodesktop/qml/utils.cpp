/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#include "utils.h"

#include <QApplication>
#include <QClipboard>
#include <QFileInfo>

#include <desktoputil/paths.h>
#include <desktoputil/stlqt.h>
#include <kulloclient/util/assert.h>

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

QColor Utils::setAlpha(const QColor &in, const qreal alpha) const
{
    const int alphaInt = qRound(alpha * 255);
    kulloAssert(alphaInt >= 0);
    kulloAssert(alphaInt <= 255);
    const QColor out(in.red(), in.green(), in.blue(), alphaInt);
    return out;
}

QString Utils::urlencode(const QString &in) const
{
    return QString(QUrl::toPercentEncoding(in));
}

QUrl Utils::defaultSaveAttachmentsDir() const
{
    return QUrl::fromLocalFile(DesktopUtil::defaultSaveAttachmentsDir());
}

}
}
