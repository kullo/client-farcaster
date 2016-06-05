/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#include "initials.h"

#include <QRegularExpression>
#include <QStringList>

namespace DesktopUtil {

namespace {
const auto WHITESPACE       = QRegularExpression(QStringLiteral("\\s"), QRegularExpression::UseUnicodePropertiesOption);
const auto NON_ALPHANUMERIC = QRegularExpression(QStringLiteral("[_\\W]"), QRegularExpression::UseUnicodePropertiesOption);

QString getFirstUnicodeCharacter(const QString &text)
{
    if (text.size() >= 2)
    {
        auto usc4 = QChar::surrogateToUcs4(text[0], text[1]);
        auto unicodeVersion = QChar::unicodeVersion(usc4);
        if (unicodeVersion != QChar::UnicodeVersion::Unicode_Unassigned)
        {
            QString out;
            out.append(text[0]);
            out.append(text[1]);
            return out;
        }
    }

    return text[0];
}
}

QString Initials::fromName(const QString &name)
{
    QStringList nameParts = name.split(WHITESPACE, QString::SkipEmptyParts);
    QStringList out;

    for (QString part : nameParts)
    {
        part.replace(NON_ALPHANUMERIC, "");
        if (!part.isEmpty())
        {
            out.append(getFirstUnicodeCharacter(part));

            if (out.size() == 2) break;
        }
    }

    return out.join("");
}

}
