/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#pragma once

#include <QString>
#include <QSharedMemory>

namespace KulloDesktop {
namespace OsIntegration {

class SingleInstanceLock
{
public:
    SingleInstanceLock(const QString &name, bool ignoreLockValue);

    bool acquire();
    void release();

private:
    QSharedMemory shared_;
    bool ignoreLockValue_;
};

}
}
