/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
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
