/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#include "scheduler.h"

#include <QQmlEngine>
#include <kulloclient/util/datetime.h>
#include <kulloclient/util/librarylogger.h>

#include "kullodesktop/qml/usersettingsmodel.h"

namespace KulloDesktop {
namespace Qml {

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

UserSettingsModel *Scheduler::user() const
{
    QQmlEngine::setObjectOwnership(user_, QQmlEngine::CppOwnership);
    return user_;
}

void Scheduler::setUser(UserSettingsModel *user)
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
