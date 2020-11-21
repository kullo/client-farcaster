/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#include "scheduler.h"

#include <QQmlEngine>
#include <kulloclient/util/datetime.h>
#include <kulloclient/util/librarylogger.h>

#include "kullodesktop/qml/usersettings.h"

namespace KulloDesktop {
namespace QmlComponents {

namespace {
const auto RUN_INTERVAL_MS = int{2 * 60 * 1000}; // 2 minutes
}

Scheduler::Scheduler(QObject *parent)
    : QObject(parent)
{
}

Scheduler::~Scheduler()
{
}

Qml::UserSettings *Scheduler::user() const
{
    QQmlEngine::setObjectOwnership(user_, QQmlEngine::CppOwnership);
    return user_;
}

void Scheduler::setUser(Qml::UserSettings *user)
{
    if (user_ != user)
    {
        user_ = user;
        emit userChanged();
    }
}

void Scheduler::start()
{
    if (timer_.isActive())
    {
        Log.e() << "Scheduler is already running. Must not be started again.";
        return;
    }

    // Pause for X minutes at program start to prioritize client update.
    // Then run every X minutes.
    connect(&timer_, &QTimer::timeout, this, &Scheduler::run);
    timer_.setTimerType(Qt::VeryCoarseTimer);
    timer_.start(RUN_INTERVAL_MS);
}

void Scheduler::run()
{
    // User tasks
    if (user_)
    {
        // MasterKey backup reminder
        if (!user_->masterKeyBackupConfirmed())
        {
            auto dontRemindBeforeStr = user_->masterKeyBackupDontRemindBefore().toStdString();
            auto dontRemindBefore = Kullo::Util::DateTime(dontRemindBeforeStr);
            auto now              = Kullo::Util::DateTime::nowUtc();
            if (dontRemindBefore <= now)
            {
                emit showBackupReminderRequested();
            }
        }
    }
}

}
}
