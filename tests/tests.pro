include(../common_pre.pri)

TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle

QT = core gui

SOURCES += \
    dice/modeltest.cpp \
    dice/test_conversation.cpp \
    dice/test_message.cpp \
    util/test_filesystem.cpp \
    util/test_httpmultipart.cpp \
    util/test_kulloaddresshelper.cpp \
    util/test_kulloclient2qt.cpp \
    util/test_kulloversion.cpp \
    util/test_osdetection.cpp \
    util/test_qtypestreamers.cpp \
    test_misc.cpp \
    main.cpp

HEADERS += \
    dice/modeltest.h \
    kullodesktoptest.h

# BEGIN desktoputil
win32:CONFIG(release, debug|release):     LIBS += -L$$OUT_PWD/../desktoputil/release/ -ldesktoputil
else:win32:CONFIG(debug, debug|release):  LIBS += -L$$OUT_PWD/../desktoputil/debug/   -ldesktoputil
else:unix:                                LIBS += -L$$OUT_PWD/../desktoputil/         -ldesktoputil

win32:CONFIG(release, debug|release):     PRE_TARGETDEPS += $$OUT_PWD/../desktoputil/release/desktoputil.lib
else:win32:CONFIG(debug, debug|release):  PRE_TARGETDEPS += $$OUT_PWD/../desktoputil/debug/desktoputil.lib
else:unix:                                PRE_TARGETDEPS += $$OUT_PWD/../desktoputil/libdesktoputil.a
# END desktoputil

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

folder_01.source = ./data
folder_01.target = .
DEPLOYMENTFOLDERS += folder_01

# BEGIN Create make target 'copyressourcefolders'
copyCommand = @echo Copying test data ...
!isEqual(PWD,$$OUT_PWD) {
    for(deploymentfolder, DEPLOYMENTFOLDERS) {
        source = $$PWD/$$eval($${deploymentfolder}.source)
        sourcePathSegments = $$split(source, /)

        win32 {
            CONFIG(debug, debug|release): MODE = debug
            CONFIG(release, debug|release): MODE = release
            target = $$OUT_PWD/$$MODE/$$eval($${deploymentfolder}.target)/$$last(sourcePathSegments)
        }
        unix {
            target = $$OUT_PWD/$$eval($${deploymentfolder}.target)
        }

        source = $$replace(source, /./, /) # Remove /./ parts
        target = $$replace(target, /./, /) # Remove /./ parts
        source = $$replace(source, //, /) # Remove consecutive slashes
        target = $$replace(target, //, /) # Remove consecutive slashes

        win32 {
            copyFrom = $$replace(source, /, \\)
            copyTo   = $$replace(target, /, \\)
            copyCommand += & @echo from: \"$$copyFrom\" to: \"$$copyTo\"
            copyCommand += & ( IF exist \"$$copyTo\" ( @echo Removing target directory ... & $(DEL_DIR) \"$$copyTo\" ) ELSE ( @echo Target does not exist yet. Perfect. ) )
            copyCommand += & $(COPY_DIR) \"$$copyFrom\" \"$$copyTo\"
        } else:unix {
            copyCommand += ; echo source: \"$$source\" target: \"$$target\"
            copyCommand += ; $(DEL_DIR) \"$$target/$$last(sourcePathSegments)\"
            copyCommand += ; $(MKDIR) \"$$target\"
            copyCommand += ; $(COPY_DIR) \"$$source\" \"$$target\"
        }
    }
}
copyressourcefolders.commands = $$copyCommand
# END Create make target 'copyressourcefolders'

QMAKE_EXTRA_TARGETS += all
QMAKE_EXTRA_TARGETS += copyressourcefolders
all.depends += copyressourcefolders

include(../common_post.pri)
