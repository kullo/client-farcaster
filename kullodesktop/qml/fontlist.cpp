/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#include "fontlist.h"

#include "desktoputil/osdetection.h"

namespace KulloDesktop {
namespace Qml {

QFont FontList::monospace() const
{
    QFont font;
    font.setStyleHint(QFont::Monospace);
    return font;
}

QFont FontList::serif() const
{
    QFont font;
    font.setStyleHint(QFont::Serif);
    return font;
}

QFont FontList::sansSerif() const
{
    QFont font;
    font.setStyleHint(QFont::SansSerif);
    return font;
}

QString FontList::monospaceFamily() const
{
    return QStringLiteral("Consolas,Menlo,Monaco,Lucida Console,Liberation Mono,DejaVu Sans Mono,Bitstream Vera Sans Mono,Courier New,monospace");
}

QString FontList::serifFamily() const
{
    QString preferred;
    if (DesktopUtil::OsDetection::linux())
    {
        preferred = QStringLiteral("'Liberation Serif'");
    }
    else if(DesktopUtil::OsDetection::windows())
    {
        preferred = QStringLiteral("Cambria");
    }
    else if(DesktopUtil::OsDetection::osx())
    {
        preferred = QStringLiteral("Times");
    }
    return preferred + "," + QStringLiteral("'Times New Roman',serif");
}

QString FontList::sansSerifFamily() const
{
    return QStringLiteral("Arial,Helvetica,sans-serif");
}

}
}
