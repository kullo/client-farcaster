include(../common_pre.pri)

QT = core

TARGET = apimirror
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    Client.cpp

HEADERS += \
    apimirror-forwards.h \
    Client.h \
    ClientAddressExistsListener.h \
    ClientCheckCredentialsListener.h \
    ClientGenerateKeysListener.h \
    RegistrationRegisterAccountListener.h

include(../common_post.pri)
