include(../common_pre.pri)

# SRC_DIR and OUT_DIR are analogues to PWD and OUT_PWD
# but use backslashes on Windows
# This must not be done in .pri files due to different values of PWD
SRC_DIR=$$shell_path($$PWD)
OUT_DIR=$$shell_path($$OUT_PWD)

QT = core gui widgets
TARGET = crashreporter
TEMPLATE = app

SOURCES += \
    crashreportdialog.cpp \
    main.cpp \
    metadata.cpp \
    uploader.cpp

HEADERS += \
    crashreportdialog.h \
    metadata.h \
    uploader.h

FORMS += \
    crashreportdialog.ui

# Copy binary to kullodesktop after building
windows() {
    windowsRelease():    FROM_DIR = "$$OUT_DIR\\release"
    else:windowsDebug(): FROM_DIR = "$$OUT_DIR\\debug"
    windowsRelease():    TO_DIR   = "$$OUT_DIR\\..\\kullodesktop\\release"
    else:windowsDebug(): TO_DIR   = "$$OUT_DIR\\..\\kullodesktop\\debug"

    QMAKE_POST_LINK = $(MKDIR) $$shell_quote($$TO_DIR) & $(COPY_FILE) $$shell_quote($$FROM_DIR\\$${TARGET}.exe) $$shell_quote($${TO_DIR}\\)
}
else {
    TO_DIR = "$$OUT_DIR/../kullodesktop"
    QMAKE_POST_LINK = $(MKDIR) $$shell_quote($$TO_DIR) && $(COPY_FILE) $$shell_quote($$OUT_DIR/$${TARGET}) $$shell_quote($${TO_DIR}/)
}

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

linux():                                 LIBS += -L$$OUT_PWD/../../build-openssl/lib -lssl -lcrypto
# END curl

# BEGIN libkullo
INCLUDEPATH += $$LIBKULLO_BIN_DIR/include
LIBS += -L$$LIBKULLO_BIN_DIR/lib \
    -lkulloclient -lboost_program_options -lz \
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
    $${TD_PREFIX}boost_program_options$${TD_SUFFIX} \
    $${TD_PREFIX}z$${TD_SUFFIX} \
    $${TD_PREFIX}boost_filesystem$${TD_SUFFIX} \
    $${TD_PREFIX}boost_system$${TD_SUFFIX}
# END libkullo

include(../common_post.pri)
