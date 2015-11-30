include(../common_pre.pri)

TEMPLATE = app

QT = core

CONFIG += console
CONFIG -= app_bundle

TARGET = registerer

HEADERS += \
    registerer.h

SOURCES += \
    main.cpp \
    registerer.cpp

unix:!macx {
    copyOpenSslCommand = @echo Copying OpenSSL ...
    copyOpenSslCommand += ; rsync -pgo --update \"$$OUT_PWD/../../build-openssl/lib/libcrypto.so.1.0.0\" \"$$OUT_PWD/../../build-openssl/lib/libssl.so.1.0.0\" \"$$OUT_PWD/\"
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

# BEGIN httpclient-curl
win32:CONFIG(release, debug|release):     LIBS += -L$$OUT_PWD/../../build-httpclient-curl/httpclient/release/     -lhttpclient
else:win32:CONFIG(debug, debug|release):  LIBS += -L$$OUT_PWD/../../build-httpclient-curl-debug/httpclient/debug/ -lhttpclient
else:unix:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../build-httpclient-curl/httpclient/             -lhttpclient
else:unix:CONFIG(debug, debug|release):   LIBS += -L$$OUT_PWD/../../build-httpclient-curl-debug/httpclient/       -lhttpclient

DEPENDPATH += $$PWD/../../httpclient-curl/httpclient

win32:CONFIG(release, debug|release):     PRE_TARGETDEPS += $$OUT_PWD/../../build-httpclient-curl/httpclient/release/httpclient.lib
else:win32:CONFIG(debug, debug|release):  PRE_TARGETDEPS += $$OUT_PWD/../../build-httpclient-curl-debug/httpclient/debug/httpclient.lib
else:unix:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../build-httpclient-curl/httpclient/libhttpclient.a
else:unix:CONFIG(debug, debug|release):   PRE_TARGETDEPS += $$OUT_PWD/../../build-httpclient-curl-debug/httpclient/libhttpclient.a

INCLUDEPATH += $$PWD/../../httpclient-curl
# END httpclient-curl

# BEGIN curl
windowsDebug(): INCLUDEPATH += $$PWD/../../build-curl-debug/include
else:           INCLUDEPATH += $$PWD/../../build-curl/include

windowsDebug(): TD_BUILD_DIR = $$PWD/../../build-curl-debug
else:           TD_BUILD_DIR = $$PWD/../../build-curl
win32 {
    TD_FILE_CURL = libcurl.lib
    TD_FILE_CURLCPP = curlcpp.lib
} else {
    TD_FILE_CURL = libcurl.a
    TD_FILE_CURLCPP = libcurlcpp.a
}
PRE_TARGETDEPS += \
    $${TD_BUILD_DIR}/lib/$${TD_FILE_CURL} \
    $${TD_BUILD_DIR}/lib/$${TD_FILE_CURLCPP}

win32: QMAKE_CXX_FLAGS += -DCURL_STATICLIB

win32:CONFIG(release, debug|release):    LIBS += -L$$OUT_PWD/../../build-curl/lib -lcurlcpp -llibcurl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../build-curl-debug/lib -lcurlcpp -llibcurl
else:macx:                               LIBS += -L$$OUT_PWD/../../build-curl/lib -lcurlcpp -lcurl -framework Security -framework Foundation -lz
else:                                    LIBS += -L$$OUT_PWD/../../build-curl/lib -lcurlcpp -lcurl -lz

unix:!macx:                              LIBS += -L$$OUT_PWD/../../build-openssl/lib -lssl -lcrypto
# END curl

# BEGIN libkullo
INCLUDEPATH += $$LIBKULLO_BIN_DIR/include
LIBS += -L$$LIBKULLO_BIN_DIR/lib \
    -lkulloclient -lbotan -ljsoncpp -lsmartsqlite -lboost_program_options \
    -lz -lboost_regex -lboost_filesystem -lboost_system

DEPENDPATH += $$LIBKULLO_BIN_DIR/include
win32 {
    TD_PREFIX = $${LIBKULLO_BIN_DIR}/lib/
    TD_SUFFIX = .lib
} else {
    TD_PREFIX = $${LIBKULLO_BIN_DIR}/lib/lib
    TD_SUFFIX = .a
}
PRE_TARGETDEPS += \
    $${TD_PREFIX}kulloclient$${TD_SUFFIX} \
    $${TD_PREFIX}botan$${TD_SUFFIX} \
    $${TD_PREFIX}jsoncpp$${TD_SUFFIX} \
    $${TD_PREFIX}smartsqlite$${TD_SUFFIX} \
    $${TD_PREFIX}boost_program_options$${TD_SUFFIX} \
    $${TD_PREFIX}z$${TD_SUFFIX} \
    $${TD_PREFIX}boost_regex$${TD_SUFFIX} \
    $${TD_PREFIX}boost_filesystem$${TD_SUFFIX} \
    $${TD_PREFIX}boost_system$${TD_SUFFIX}
# END libkullo

include(../common_post.pri)
