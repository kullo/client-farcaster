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

# BEGIN desktoputil
win32:CONFIG(release, debug|release):     LIBS += -L$$OUT_PWD/../desktoputil/release/ -ldesktoputil
else:win32:CONFIG(debug, debug|release):  LIBS += -L$$OUT_PWD/../desktoputil/debug/   -ldesktoputil
else:unix:                                LIBS += -L$$OUT_PWD/../desktoputil/         -ldesktoputil

win32:CONFIG(release, debug|release):     PRE_TARGETDEPS += $$OUT_PWD/../desktoputil/release/desktoputil.lib
else:win32:CONFIG(debug, debug|release):  PRE_TARGETDEPS += $$OUT_PWD/../desktoputil/debug/desktoputil.lib
else:unix:                                PRE_TARGETDEPS += $$OUT_PWD/../desktoputil/libdesktoputil.a
# END desktoputil

# BEGIN httpclient-curl
CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../bin-httpclient-curl/lib/       -lhttpclient
CONFIG(debug, debug|release):   LIBS += -L$$OUT_PWD/../../bin-httpclient-curl-debug/lib/ -lhttpclient

CONFIG(release, debug|release): DEPENDPATH += $$PWD/../../bin-httpclient-curl/lib
CONFIG(debug, debug|release):   DEPENDPATH += $$PWD/../../bin-httpclient-curl-debug/lib

win32:CONFIG(release, debug|release):     PRE_TARGETDEPS += $$OUT_PWD/../../bin-httpclient-curl/lib/httpclient.lib
else:win32:CONFIG(debug, debug|release):  PRE_TARGETDEPS += $$OUT_PWD/../../bin-httpclient-curl-debug/lib/httpclient.lib
else:unix:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../bin-httpclient-curl/lib/libhttpclient.a
else:unix:CONFIG(debug, debug|release):   PRE_TARGETDEPS += $$OUT_PWD/../../bin-httpclient-curl-debug/lib/libhttpclient.a

CONFIG(release, debug|release): INCLUDEPATH += $$PWD/../../bin-httpclient-curl/include
CONFIG(debug, debug|release):   INCLUDEPATH += $$PWD/../../bin-httpclient-curl-debug/include
# END httpclient-curl

# BEGIN libkullo
INCLUDEPATH += $$LIBKULLO_BIN_DIR/include
LIBS += -L$$LIBKULLO_BIN_DIR/lib \
    -lkulloclient -lbotan -ljsoncpp -lgmock -lsmartsqlite \
    -lboost_program_options -lz -lboost_regex \
    -lboost_filesystem -lboost_system

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
    $${TD_PREFIX}gmock$${TD_SUFFIX} \
    $${TD_PREFIX}smartsqlite$${TD_SUFFIX} \
    $${TD_PREFIX}boost_program_options$${TD_SUFFIX} \
    $${TD_PREFIX}z$${TD_SUFFIX} \
    $${TD_PREFIX}boost_regex$${TD_SUFFIX} \
    $${TD_PREFIX}boost_filesystem$${TD_SUFFIX} \
    $${TD_PREFIX}boost_system$${TD_SUFFIX}
# END libkullo

# BEGIN curl
windowsDebug(): INCLUDEPATH += $$PWD/../../build-curl-debug/include
else:           INCLUDEPATH += $$PWD/../../build-curl/include

win32: QMAKE_CXX_FLAGS += -DCURL_STATICLIB

win32:CONFIG(release, debug|release):    LIBS += -L$$OUT_PWD/../../build-curl/lib -lcurlcpp -llibcurl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../build-curl-debug/lib -lcurlcpp -llibcurl
else:macx:                               LIBS += -L$$OUT_PWD/../../build-curl/lib -lcurlcpp -lcurl -framework Security -framework Foundation -lz
else:                                    LIBS += -L$$OUT_PWD/../../build-curl/lib -lcurlcpp -lcurl -lz

linux():                                 LIBS += -L$$OUT_PWD/../../build-openssl/lib -lssl -lcrypto
# END curl

include(../common_post.pri)
