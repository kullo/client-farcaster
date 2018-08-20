include(../common_pre.pri)

TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle

QT = core gui

SOURCES += \
    main.cpp \
    test_networking.cpp

HEADERS += \
    integrationtest.h

linux() {
    linuxRelease(): OPENSSL_BUILD_DIR = $$BUILD_ROOT/../build-linux-x86_64-release/build-openssl
    linuxDebug():   OPENSSL_BUILD_DIR = $$BUILD_ROOT/../build-linux-x86_64-debug/build-openssl
    copyOpenSslCommand = @echo Copying OpenSSL ...
    copyOpenSslCommand += ; rsync -pgo --update \"$$OPENSSL_BUILD_DIR/lib/libcrypto.so.1.0.0\" \"$$OPENSSL_BUILD_DIR/lib/libssl.so.1.0.0\" \"$$OUT_PWD/\"
    copyOpenSsl.commands = $$copyOpenSslCommand
    QMAKE_EXTRA_TARGETS += copyOpenSsl
    all.depends += copyOpenSsl
}

QMAKE_EXTRA_TARGETS += all

# BEGIN desktoputil
win32:CONFIG(release, debug|release):     LIBS += -L$$OUT_PWD/../desktoputil/release/ -ldesktoputil
else:win32:CONFIG(debug, debug|release):  LIBS += -L$$OUT_PWD/../desktoputil/debug/   -ldesktoputil
else:unix:                                LIBS += -L$$OUT_PWD/../desktoputil/         -ldesktoputil

win32:CONFIG(release, debug|release):     PRE_TARGETDEPS += $$OUT_PWD/../desktoputil/release/desktoputil.lib
else:win32:CONFIG(debug, debug|release):  PRE_TARGETDEPS += $$OUT_PWD/../desktoputil/debug/desktoputil.lib
else:unix:                                PRE_TARGETDEPS += $$OUT_PWD/../desktoputil/libdesktoputil.a
# END desktoputil

# include order is important for linking order
include(../qmake-modules/httpclient-curl.pri)
include(../qmake-modules/libkullo.pri)
include(../qmake-modules/botan.pri)
include(../qmake-modules/curl.pri)

include(../common_post.pri)
