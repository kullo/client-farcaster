/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
#pragma once

#include <memory>
#include <QString>
#include <kulloclient/kulloclient-forwards.h>

namespace DesktopUtil {

class DatabaseFiles
{
public:
    explicit DatabaseFiles(const QString &dbFilenameTemplate);
    QString databaseFilepath(const Kullo::Api::Address &address) const;
    void prepareDatabaseFolder(const Kullo::Api::Address &address) const;
    void removeDatabase(const Kullo::Api::Address &address) const;

private:
    QString dbFilenameTemplate_;
};

}
