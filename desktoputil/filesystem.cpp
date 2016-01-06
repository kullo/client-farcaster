/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#include "filesystem.h"

#include <string>
#include <QRegularExpression>

#include "desktoputil/qtypestreamers.h"

static const std::string ALLOWED_FILENAME_CHARACTERS_WIN  = R"(a-zA-Z0-9)";
static const std::string ALLOWED_FILENAME_CHARACTERS_UNIX = R"(\w)";
static const std::string ALLOWED_FILENAME_SPECIALS_WIN    = R"(-. _)";
static const std::string ALLOWED_FILENAME_SPECIALS_UNIX   = R"(-. _\:?%*|"<>^)";

#ifdef Q_OS_WIN
static const std::string ALLOWED_FILENAME_CHARACTERS = ALLOWED_FILENAME_CHARACTERS_WIN;
static const std::string ALLOWED_FILENAME_SPECIALS   = ALLOWED_FILENAME_SPECIALS_WIN;
#else
static const std::string ALLOWED_FILENAME_CHARACTERS = ALLOWED_FILENAME_CHARACTERS_UNIX;
static const std::string ALLOWED_FILENAME_SPECIALS   = ALLOWED_FILENAME_SPECIALS_UNIX;
#endif

static const QString PREPARE_TMP_FILENAME_PATTERN = QStringLiteral("[^%1%2]")
        .arg(QString::fromStdString(ALLOWED_FILENAME_CHARACTERS))
        .arg(QRegularExpression::escape(QString::fromStdString(ALLOWED_FILENAME_SPECIALS)))
        ;

static const QRegularExpression PREPARE_TMP_FILENAME_REGEXP(PREPARE_TMP_FILENAME_PATTERN, QRegularExpression::UseUnicodePropertiesOption);

namespace DesktopUtil {

QString Filesystem::prepareTmpFilename(const QString &original)
{
    QString out = original;
    out.replace(PREPARE_TMP_FILENAME_REGEXP, "_");
    return out;
}

}
