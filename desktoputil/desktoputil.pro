include(../common_pre.pri)

QT = core

TARGET = desktoputil
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    dice/model/accountinfo.cpp \
    dice/model/attachment.cpp \
    dice/model/client.cpp \
    dice/model/conversations.cpp \
    dice/model/conversation.cpp \
    dice/model/draft.cpp \
    dice/model/draftattachment.cpp \
    dice/model/message.cpp \
    dice/model/participant.cpp \
    dice/sync/synccontroller.cpp \
    dice/sync/syncerqobject.cpp \
    asynchttpgetmanager.cpp \
    asynchttppostmanager.cpp \
    asynctask.cpp \
    filesystem.cpp \
    httpmultipart.cpp \
    kulloaddresshelper.cpp \
    kulloclient2qt.cpp \
    kulloversion.cpp \
    osdetection.cpp \
    qtypestreamers.cpp \
    stlqt.cpp \
    versions.cpp

HEADERS += \
    dice/model/accountinfo.h \
    dice/model/attachment.h \
    dice/model/client.h \
    dice/model/conversations.h \
    dice/model/conversation.h \
    dice/model/draft.h \
    dice/model/draftattachment.h \
    dice/model/exceptions.h \
    dice/model/message.h \
    dice/model/participant.h \
    dice/sync/synccontroller.h \
    dice/sync/syncerqobject.h \
    dice/dice-forwards.h \
    asynchttpgetmanager.h \
    asynchttppostmanager.h \
    asynctask.h \
    filesystem.h \
    httpmultipart.h \
    kulloaddresshelper.h \
    kulloclient2qt.h \
    kulloversion.h \
    metatypes.h \
    osdetection.h \
    qtypestreamers.h \
    stlqt.h \
    versions.h

# BEGIN httpclient-curl
INCLUDEPATH += $$PWD/../../httpclient-curl
# END httpclient-curl

include(../common_post.pri)
