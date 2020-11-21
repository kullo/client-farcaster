/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#include "stabletimer.h"

namespace KulloDesktop {
namespace QmlComponents {

StableTimer::StableTimer(QObject *parent)
    : QObject(parent)
{
    impl_.setInterval(1000); // 1000 ms
    impl_.setSingleShot(true); // repeat: false

    connect(&impl_, &QTimer::timeout,
            this, &StableTimer::triggered);
}

int StableTimer::interval() const
{
    return impl_.interval();
}

void StableTimer::setInterval(const int msec)
{
    if (msec == interval()) return;

    impl_.setInterval(msec);
    emit intervalChanged();
}

bool StableTimer::repeat() const
{
    return !impl_.isSingleShot();
}

void StableTimer::setRepeat(const bool repeat)
{
    if (!impl_.isSingleShot() == repeat) return;

    impl_.setSingleShot(!repeat);
    emit repeatChanged();
}

bool StableTimer::running() const
{
    return impl_.isActive();
}

void StableTimer::setRunning(const bool running)
{
    if (impl_.isActive() == running) return;

    if (running)
    {
        start();
    }
    else
    {
        stop();
    }
}

bool StableTimer::triggeredOnStart() const
{
    return triggeredOnStart_;
}

void StableTimer::setTriggeredOnStart(const bool triggeredOnStart)
{
    if (triggeredOnStart_ == triggeredOnStart) return;

    triggeredOnStart_ = triggeredOnStart;
    emit triggeredOnStartChanged();
}

void StableTimer::restart()
{
    stop();
    start();
}

void StableTimer::start()
{
    preStartActions();
    impl_.start();
}

void StableTimer::stop()
{
    impl_.stop();
}

void StableTimer::preStartActions()
{
    if (triggeredOnStart_)
    {
        emit triggered();
    }
}

}
}
