/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#include "initials.h"

#include <QRegularExpression>
#include <QStringList>

namespace DesktopUtil {

namespace {
const auto WORD = QRegularExpression(QStringLiteral("[[:alnum:]](?:-|\\w)*"), QRegularExpression::UseUnicodePropertiesOption);

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
    auto matchIter = WORD.globalMatch(name.toUpper());
    QStringList words;
    while (matchIter.hasNext())
    {
        words << matchIter.next().captured(0);
    }
    QString out;
    if (words.size() >= 1)
    {
        out += getFirstUnicodeCharacter(words[0]);
    }
    if (words.size() >= 2)
    {
        out += getFirstUnicodeCharacter(words[words.size() - 1]);
    }
    return out;
}

}
