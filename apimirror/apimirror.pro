include(../common_pre.pri)

QT = core

TARGET = apimirror
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    Client.cpp \
    eventdispatcher.cpp

HEADERS += \
    enums/DraftPartHolder.h \
    enums/LocalErrorHolder.h \
    enums/NetworkErrorHolder.h \
    enums/SyncPhaseHolder.h \
    apimirror-forwards.h \
    Client.h \
    ClientAddressExistsListener.h \
    ClientCheckCredentialsListener.h \
    ClientCreateSessionListener.h \
    ClientGenerateKeysListener.h \
    DraftAttachmentsAddListener.h \
    DraftAttachmentsContentListener.h \
    DraftAttachmentsSaveToListener.h \
    eventdispatcher.h \
    MessageAttachmentsContentListener.h \
    MessageAttachmentsSaveToListener.h \
    RegistrationRegisterAccountListener.h \
    SessionAccountInfoListener.h \
    SessionListener.h \
    SyncerListener.h

include(../common_post.pri)
