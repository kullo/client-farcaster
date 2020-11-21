/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#pragma once

#include <QObject>

#include <desktoputil/osdetection.h>

namespace KulloDesktop {
namespace Qml {

class Os : public QObject
{
    Q_OBJECT

public:
    explicit Os(QObject *parent = nullptr) : QObject(parent) {}

    Q_PROPERTY(QString familyPretty READ familyPretty NOTIFY familyPrettyChanged)
    static QString familyPretty() { return DesktopUtil::OsDetection::familyPretty(); }

    Q_PROPERTY(bool linux READ linux NOTIFY linuxChanged)
    static bool linux() { return DesktopUtil::OsDetection::linux(); }

    Q_PROPERTY(bool osx READ osx NOTIFY osxChanged)
    static bool osx() { return DesktopUtil::OsDetection::osx(); }

    Q_PROPERTY(bool windows READ windows NOTIFY windowsChanged)
    static bool windows() { return DesktopUtil::OsDetection::windows(); }

    Q_PROPERTY(bool debug READ debug NOTIFY debugChanged)
    static bool debug() { return DesktopUtil::OsDetection::debug(); }

signals:
    // unused signals
    void familyPrettyChanged();
    void linuxChanged();
    void osxChanged();
    void windowsChanged();
    void debugChanged();
};

}
}
