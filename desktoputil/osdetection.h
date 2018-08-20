/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
#pragma once

#include <QtGlobal>

#if defined Q_OS_LINUX
    #define K_OS_LINUX 1
    #define K_OS_FAMILY "linux"
    #define K_OS_FAMILY_PRETTY "Linux"
    #if defined Q_PROCESSOR_X86_32
        #define K_OS_LINUX32 1
        #define K_OS_NAME "linux32"
    #elif defined Q_PROCESSOR_X86_64
        #define K_OS_LINUX64 1
        #define K_OS_NAME "linux64"
    #endif
#elif defined Q_OS_WIN
    #define K_OS_WINDOWS 1
    #define K_OS_NAME "windows"
    #define K_OS_FAMILY "windows"
    #define K_OS_FAMILY_PRETTY "Windows"
#elif defined Q_OS_OSX
    #define K_OS_OSX 1
    #define K_OS_NAME "osx"
    #define K_OS_FAMILY "osx"
    #define K_OS_FAMILY_PRETTY "OS X"
#else
    #error "OS unknown"
#endif

#if defined K_OS_LINUX || defined K_OS_WINDOWS
    #define K_HAS_BREAKPAD 1
#endif

namespace DesktopUtil {

class OsDetection
{
public:
    static QString familyPretty();
    static QString name();
    static bool linux();
    static bool osx();
    static bool windows();
    static bool debug();
};

}
