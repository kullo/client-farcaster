/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <memory>
#include <QString>
#include <kulloclient/kulloclient-forwards.h>

namespace DesktopUtil {

class DatabaseFiles
{
public:
    explicit DatabaseFiles(const QString &dbFilenameTemplate);
    QString databaseFilepath(const std::shared_ptr<Kullo::Api::Address> &address) const;
    void prepareDatabaseFolder(const std::shared_ptr<Kullo::Api::Address> &address) const;
    void removeDatabase(const std::shared_ptr<Kullo::Api::Address> &address) const;

private:
    QString dbFilenameTemplate_;
};

}
