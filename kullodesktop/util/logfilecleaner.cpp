/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
#include "logfilecleaner.h"

#include <QDir>
#include <desktoputil/kulloversion.h>
#include <desktoputil/paths.h>
#include <desktoputil/qtypestreamers.h>
#include <desktoputil/versions.h>
#include <kulloclient/util/librarylogger.h>
#include <kulloclient/util/version.h>

namespace KulloDesktop {
namespace Util {

void LogfileCleaner::clean()
{
    auto dir = QDir(DesktopUtil::logfileDir());

    QFile::remove(QFileInfo(dir, "kullo_log.html").absoluteFilePath());

    QFileInfoList removeFiles;
    QFileInfoList logfiles = dir.entryInfoList(QStringList() << "kullo_log_*.html");
    for (QFileInfo logfile : logfiles)
    {
        QString filename = logfile.fileName();
        QString suffix = logfile.suffix();
        if (filename.left(10) == "kullo_log_" && suffix == "html")
        {
            QString version = filename.mid(10, filename.length()-10-5);
            try {
                DesktopUtil::KulloVersion kulloVersion(version.toStdString());
                if (kulloVersion < DesktopUtil::Versions::kullodesktopVersion())
                {
                    removeFiles.append(logfile);
                }
            }
            catch (...)
            {
                Log.w() << "Kullo version invalid: " << version << " "
                        << "in " << filename;
            }
        }
    }
    Log.i() << "Logfiles to be deleted " << removeFiles;

    for (const auto &file : removeFiles)
    {
        if (QFile::remove(file.absoluteFilePath()))
        {
            Log.i() << "Deleted: " << file;
        }
        else
        {
            Log.w() << "File could not be deleted: " << file;
        }
    }
}

}
}
