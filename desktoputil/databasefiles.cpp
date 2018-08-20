/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
#include "databasefiles.h"

#include <QDir>
#include <QFileInfo>

#include <desktoputil/qtypestreamers.h>
#include <kulloclient/api_impl/Address.h>
#include <kulloclient/util/librarylogger.h>

namespace DesktopUtil {

DatabaseFiles::DatabaseFiles(const QString &dbFilenameTemplate)
    : dbFilenameTemplate_(dbFilenameTemplate)
{
}

QString DatabaseFiles::databaseFilepath(const Kullo::Api::Address &address) const
{
    return dbFilenameTemplate_.arg(QString::fromStdString(address.toString()));
}

void DatabaseFiles::prepareDatabaseFolder(const Kullo::Api::Address &address) const
{
    const QDir dbParentDirectoryAbsolute = QFileInfo(databaseFilepath(address)).absoluteDir();

    // TODO: Make check compatible with symbolic links
    if (!dbParentDirectoryAbsolute.exists())
    {
        Log.i() << "Creating database directory: " << dbParentDirectoryAbsolute;
        if (!QDir().mkpath(dbParentDirectoryAbsolute.path()))
        {
            Log.f() << "Could not create database directory: " << dbParentDirectoryAbsolute;
        }
    }

    if (!QFileInfo(dbParentDirectoryAbsolute.path()).isWritable())
    {
        Log.f() << "Database directory not writeable: " << dbParentDirectoryAbsolute;
    }
}

void DatabaseFiles::removeDatabase(const Kullo::Api::Address & address) const
{
    const QString dbFile = databaseFilepath(address);
    const QString dbFileShm = dbFile + "-shm";
    const QString dbFileWal = dbFile + "-wal";

    Log.i() << "Removing database file " << dbFile << " ...";

    if (QFile::remove(dbFile))    Log.i() << "Removed database file: " << dbFile;
    if (QFile::remove(dbFileShm)) Log.i() << "Removed database file: " << dbFileShm;
    if (QFile::remove(dbFileWal)) Log.i() << "Removed database file: " << dbFileWal;
}

}
