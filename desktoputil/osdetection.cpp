/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#include "osdetection.h"

#include <QString>

namespace DesktopUtil {

QString OsDetection::familyPretty()
{
    return QString(K_OS_FAMILY_PRETTY);
}

QString OsDetection::name()
{
    return QString(K_OS_NAME);
}

bool OsDetection::linux()
{
#ifdef K_OS_LINUX
    return true;
#else
    return false;
#endif
}

bool OsDetection::osx()
{
#ifdef K_OS_OSX
    return true;
#else
    return false;
#endif
}

bool OsDetection::windows()
{
#ifdef K_OS_WINDOWS
    return true;
#else
    return false;
#endif
}

bool OsDetection::debug()
{
#ifdef QT_NO_DEBUG
    return false; // release
#else
    return true; // debug
#endif
}

}
