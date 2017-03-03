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

# include order is important for linking order
include(../qmake-modules/httpclient-curl.pri)
include(../qmake-modules/libkullo.pri)
include(../qmake-modules/curl.pri)
include(../qmake-modules/botan.pri)

include(../common_post.pri)
