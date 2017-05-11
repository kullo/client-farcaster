include(../common_pre.pri)

QT = core

TARGET = desktoputil
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    asynchttpgetmanager.cpp \
    asynchttppostmanager.cpp \
    asynctask.cpp \
    breakpadsetup.cpp \
    databasefiles.cpp \
    filesystem.cpp \
    initials.cpp \
    kulloaddresshelper.cpp \
    kulloclient2qt.cpp \
    kulloversion.cpp \
    osdetection.cpp \
    paths.cpp \
    qdebugmessagehandler.cpp \
    qtypestreamers.cpp \
    stlqt.cpp \
    threadblocker.cpp \
    versions.cpp

HEADERS += \
    asynchttpgetmanager.h \
    asynchttppostmanager.h \
    asynctask.h \
    breakpadsetup.h \
    databasefiles.h \
    filesystem.h \
    initials.h \
    kulloaddresshelper.h \
    kulloclient2qt.h \
    kulloversion.h \
    metatypes.h \
    osdetection.h \
    paths.h \
    qdebugmessagehandler.h \
    qtypestreamers.h \
    stlqt.h \
    threadblocker.h \
    versions.h \
    desktoputil-forwards.h

# BEGIN httpclient-curl
INCLUDEPATH += $$CMAKE_DEPENDENCIES_ROOT/bin-httpclient-curl/include
# END httpclient-curl

include(../common_post.pri)
