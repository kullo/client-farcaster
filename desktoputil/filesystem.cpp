/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
#include "filesystem.h"

#include <string>
#include <QDir>
#include <QRegularExpression>
#include <kulloclient/util/assert.h>

#include "desktoputil/qtypestreamers.h"
#include "desktoputil/osdetection.h"

namespace DesktopUtil {

namespace {
const std::string ALLOWED_FILENAME_CHARACTERS_WIN  = R"(\w)"; // Anything unicode calls L (letter) or N (digit), plus underscore
const std::string ALLOWED_FILENAME_CHARACTERS_UNIX = R"(\w)"; // Anything unicode calls L (letter) or N (digit), plus underscore
const std::string ALLOWED_FILENAME_SPECIALS_WIN    = R"(-. _)";
const std::string ALLOWED_FILENAME_SPECIALS_UNIX   = R"(-. _\:?%*|"<>^)";
const std::string ALLOWED_FILENAME_SPECIALS_LATIN1 = R"(¡¢£¤¥¦§¨©«¬®¯°±´¶·¸»¿×÷)"; // latin1 special chars not covered by \w

#ifdef Q_OS_WIN
const std::string ALLOWED_FILENAME_CHARACTERS = ALLOWED_FILENAME_CHARACTERS_WIN;
const std::string ALLOWED_FILENAME_SPECIALS   = ALLOWED_FILENAME_SPECIALS_LATIN1 + ALLOWED_FILENAME_SPECIALS_WIN;
#else
const std::string ALLOWED_FILENAME_CHARACTERS = ALLOWED_FILENAME_CHARACTERS_UNIX;
const std::string ALLOWED_FILENAME_SPECIALS   = ALLOWED_FILENAME_SPECIALS_LATIN1 + ALLOWED_FILENAME_SPECIALS_UNIX;
#endif

const QString PREPARE_TMP_FILENAME_PATTERN = QStringLiteral("[^%1%2]")
        .arg(QString::fromStdString(ALLOWED_FILENAME_CHARACTERS))
        .arg(QRegularExpression::escape(QString::fromStdString(ALLOWED_FILENAME_SPECIALS)))
        ;

const QRegularExpression PREPARE_TMP_FILENAME_REGEXP(PREPARE_TMP_FILENAME_PATTERN,
                                                     QRegularExpression::UseUnicodePropertiesOption);
}

QString Filesystem::prepareTmpFilename(const QString &original)
{
    QString out = original;
    out.replace(PREPARE_TMP_FILENAME_REGEXP, "_");
    return out;
}

std::string Filesystem::shortenSourcePath(const std::string &filepath)
{
    const auto PATH_SEPARATORS = R"(/\)";

    std::size_t pos = filepath.find_last_of(PATH_SEPARATORS);
    if (pos != std::string::npos)
    {
        return filepath.substr(pos + 1);
    }
    else
    {
        return filepath;
    }
}

QUrl Filesystem::increaseFilenameCounter(const QUrl &file)
{
    kulloAssert(file.isLocalFile());

    QFileInfo fi(file.toLocalFile());

    QString base = fi.baseName();

    QRegularExpression re(R"((.*) \(([1-9][0-9]*)\))");
    auto match = re.match(base);
    if (match.hasMatch())
    {
        QString number = match.captured(2);
        QString newNumber = QString::number(number.toInt() + 1);
        base = match.captured(1) + " (" + newNumber + ")";
    }
    else
    {
        base = base + " (1)";
    }

    QString filename = base + "." + fi.completeSuffix();

    return QUrl::fromLocalFile(fi.dir().filePath(filename));
}

}
