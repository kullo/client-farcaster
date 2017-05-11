include(../common_pre.pri)

# SRC_DIR and OUT_DIR are analogues to PWD and OUT_PWD
# but use backslashes on Windows
# This must not be done in .pri files due to different values of PWD
SRC_DIR=$$shell_path($$PWD)
OUT_DIR=$$shell_path($$OUT_PWD)

QT = core gui qml quick widgets \
    core-private gui-private \
    multimedia printsupport

TEMPLATE = app

RESOURCES += \
    resources.qrc \
    translations.qrc \
    ui.qrc

windows() {
    TARGET = kullo

    # RC_FILE must not be set:
    # http://doc.qt.io/qt-5/qmake-variable-reference.html#qmake-target-company
    # Special chars like Em-Dash are not supported
    RC_ICONS = kullo.ico
    QMAKE_TARGET_COMPANY = Kullo GmbH
    QMAKE_TARGET_PRODUCT = Kullo
    QMAKE_TARGET_DESCRIPTION = Kullo - Secure Communication
    QMAKE_TARGET_COPYRIGHT = Kullo GmbH
}
osx() {
    TARGET = Kullo
    ICON = kullo.icns
    QMAKE_INFO_PLIST = Info.plist
}
linux() {
    CONFIG(debug, debug|release) {
        TARGET = kullo-debug
    }
    CONFIG(release, debug|release) {
        TARGET = kullo
    }
}

linux() {
    copyOpenSslCommand = @echo Copying OpenSSL ...
    copyOpenSslCommand += ; rsync -pgo --update \"$$BUILD_ROOT/../build-openssl/lib/libcrypto.so.1.0.0\" \"$$BUILD_ROOT/../build-openssl/lib/libssl.so.1.0.0\" \"$$OUT_PWD/\"
    copyOpenSsl.commands = $$copyOpenSslCommand
    QMAKE_EXTRA_TARGETS += copyOpenSsl
    all.depends += copyOpenSsl
}

copyLicensesCommand = @echo Copying licenses file ...
windowsRelease(): copyLicensesCommand += & $(MKDIR) \"$$OUT_PWD/release/assets/\" & $(COPY_FILE) \"$$PWD\\assets\\licenses.html\" \"$$OUT_PWD/release/assets/\"
windowsDebug():   copyLicensesCommand += & $(MKDIR) \"$$OUT_PWD/debug/assets/\"   & $(COPY_FILE) \"$$PWD\\assets\\licenses.html\" \"$$OUT_PWD/debug/assets/\"
osx():            copyLicensesCommand += ; $(MKDIR) \"$$OUT_PWD/$${TARGET}.app/Contents/Resources/assets/\" ; $(COPY_FILE) \"$$PWD/assets/licenses.html\" \"$$OUT_PWD/$${TARGET}.app/Contents/Resources/assets/\"
linux():          copyLicensesCommand += ; $(MKDIR) \"$$OUT_PWD/assets/\" ; $(COPY_FILE) \"$$PWD/assets/licenses.html\" \"$$OUT_PWD/assets/\"
copyLicenses.commands = $$copyLicensesCommand

copyTestUiCommand = @echo Copying Test Binary and Test UI ...
windows() {
    copyTestUiCommand += & $(COPY_FILE) $$shell_quote($$SRC_DIR\\test_binary.py) $$shell_quote($$OUT_DIR\\)
    copyTestUiCommand += & $(COPY_FILE) $$shell_quote($$SRC_DIR\\test_ui.py)     $$shell_quote($$OUT_DIR\\)
}
else {
    copyTestUiCommand += ; $(COPY_FILE) $$shell_quote($$SRC_DIR/test_binary.py) $$shell_quote($$OUT_DIR/)
    copyTestUiCommand += ; $(COPY_FILE) $$shell_quote($$SRC_DIR/test_ui.py)     $$shell_quote($$OUT_DIR/)
}
copyTestUi.commands = $$copyTestUiCommand

SOURCES += \
    applications/exceptionawareqapplication.cpp \
    applications/kulloapplication.cpp \
    applications/osxkulloapplication.cpp \
    imageproviders/abstractavatarprovider.cpp \
    imageproviders/attachmentpreviewprovider.cpp \
    imageproviders/conversationavatarprovider.cpp \
    imageproviders/messagesenderavatarprovider.cpp \
    imageproviders/participantavatarprovider.cpp \
    imageproviders/usersettingsavatarprovider.cpp \
    osintegration/kullotrayicon.cpp \
    osintegration/programoptions.cpp \
    osintegration/singleinstancelock.cpp \
    qml/attachmentlistmodel.cpp \
    qml/attachmentmodel.cpp \
    qml/conversationlistmodel.cpp \
    qml/conversationlistsource.cpp \
    qml/conversationmodel.cpp \
    qml/devicesettings.cpp \
    qml/draftattachmentlistmodel.cpp \
    qml/draftattachmentmodel.cpp \
    qml/draftmodel.cpp \
    qml/fontlist.cpp \
    qml/hdpi.cpp \
    qml/inbox.cpp \
    qml/innerapplication.cpp \
    qml/kulloversionchecker.cpp \
    qml/messagemodel.cpp \
    qml/messagelistmodel.cpp \
    qml/messagelistsource.cpp \
    qml/modalitywatcher.cpp \
    qml/sender.cpp \
    qml/softwareversions.cpp \
    qml/usersettings.cpp \
    qml/utils.cpp \
    qml_bridge/qmlsetup.cpp \
    qml_components/visuals/advancedrectangle.cpp \
    qml_components/visuals/hint.cpp \
    qml_components/accountinfo.cpp \
    qml_components/attachmentsadder.cpp \
    qml_components/changelog.cpp \
    qml_components/existencechecker.cpp \
    qml_components/fileopendialog.cpp \
    qml_components/filesavedialog.cpp \
    qml_components/kulloupdatermodel.cpp \
    qml_components/libraryloggerqmlconnector.cpp \
    qml_components/loginchecker.cpp \
    qml_components/registerer.cpp \
    qml_components/scheduler.cpp \
    qml_components/stabletimer.cpp \
    util/consoleextendedloglistener.cpp \
    util/htmlfileloglistener.cpp \
    util/kullofoldersetup.cpp \
    util/logfilecleaner.cpp \
    main.cpp

HEADERS += \
    applications/exceptionawareqapplication.h \
    applications/kulloapplication.h \
    applications/osxkulloapplication.h \
    imageproviders/abstractavatarprovider.h \
    imageproviders/attachmentpreviewprovider.h \
    imageproviders/conversationavatarprovider.h \
    imageproviders/messagesenderavatarprovider.h \
    imageproviders/participantavatarprovider.h \
    imageproviders/usersettingsavatarprovider.h \
    osintegration/kullotrayicon.h \
    osintegration/programoptions.h \
    osintegration/singleinstancelock.h \
    qml/attachmentmodel.h \
    qml/attachmentlistmodel.h \
    qml/conversationlistmodel.h \
    qml/conversationlistsource.h \
    qml/conversationmodel.h \
    qml/devicesettings.h \
    qml/draftattachmentlistmodel.h \
    qml/draftattachmentmodel.h \
    qml/draftmodel.h \
    qml/fontlist.h \
    qml/hdpi.h \
    qml/inbox.h \
    qml/innerapplication.h \
    qml/kulloversionchecker.h \
    qml/messagemodel.h \
    qml/messagelistmodel.h \
    qml/messagelistsource.h \
    qml/modalitywatcher.h \
    qml/os.h \
    qml/sender.h \
    qml/softwareversions.h \
    qml/usersettings.h \
    qml/utils.h \
    qml_bridge/qmlsetup.h \
    qml_components/visuals/advancedrectangle.h \
    qml_components/visuals/hint.h \
    qml_components/accountinfo.h \
    qml_components/attachmentsadder.h \
    qml_components/changelog.h \
    qml_components/existencechecker.h \
    qml_components/fileopendialog.h \
    qml_components/filesavedialog.h \
    qml_components/kulloupdatermodel.h \
    qml_components/libraryloggerqmlconnector.h \
    qml_components/loginchecker.h \
    qml_components/registerer.h \
    qml_components/scheduler.h \
    qml_components/stabletimer.h \
    util/consoleextendedloglistener.h \
    util/htmlfileloglistener.h \
    util/kullofoldersetup.h \
    util/logfilecleaner.h \
    farcaster-forwards.h

# BEGIN apimirror
win32:CONFIG(release, debug|release):     LIBS += -L$$OUT_PWD/../apimirror/release/ -lapimirror
else:win32:CONFIG(debug, debug|release):  LIBS += -L$$OUT_PWD/../apimirror/debug/   -lapimirror
else:unix:                                LIBS += -L$$OUT_PWD/../apimirror/         -lapimirror

win32:CONFIG(release, debug|release):     PRE_TARGETDEPS += $$OUT_PWD/../apimirror/release/apimirror.lib
else:win32:CONFIG(debug, debug|release):  PRE_TARGETDEPS += $$OUT_PWD/../apimirror/debug/apimirror.lib
unix:                                     PRE_TARGETDEPS += $$OUT_PWD/../apimirror/libapimirror.a
# END apimirror

# BEGIN desktoputil
win32:CONFIG(release, debug|release):     LIBS += -L$$OUT_PWD/../desktoputil/release/ -ldesktoputil
else:win32:CONFIG(debug, debug|release):  LIBS += -L$$OUT_PWD/../desktoputil/debug/   -ldesktoputil
else:unix:                                LIBS += -L$$OUT_PWD/../desktoputil/         -ldesktoputil

win32:CONFIG(release, debug|release):     PRE_TARGETDEPS += $$OUT_PWD/../desktoputil/release/desktoputil.lib
else:win32:CONFIG(debug, debug|release):  PRE_TARGETDEPS += $$OUT_PWD/../desktoputil/debug/desktoputil.lib
else:unix:                                PRE_TARGETDEPS += $$OUT_PWD/../desktoputil/libdesktoputil.a
# END desktoputil

# BEGIN breakpadwrapper
windows() | linux() {
    windows(): LIBS += -lshell32 -lole32
    LIBS += -lbreakpadwrapper
}
# END breakpadwrapper

# include order is important for linking order
include(../qmake-modules/httpclient-curl.pri)
include(../qmake-modules/libkullo.pri)
include(../qmake-modules/botan.pri)
include(../qmake-modules/curl.pri)

# BEGIN breakpad
win32 {
    CONFIG(debug, debug|release) {
        LIBS += -L$$PWD/../../build-breakpad/src/client/windows/handler/Debug/lib -lexception_handler
        LIBS += -L$$PWD/../../build-breakpad/src/client/windows/crash_generation/Debug/lib -lcrash_generation_client
        LIBS += -L$$PWD/../../build-breakpad/src/client/windows/Debug/lib -lcommon
    }
    CONFIG(release, debug|release) {
        LIBS += -L$$PWD/../../build-breakpad/src/client/windows/handler/Release/lib -lexception_handler
        LIBS += -L$$PWD/../../build-breakpad/src/client/windows/crash_generation/Release/lib -lcrash_generation_client
        LIBS += -L$$PWD/../../build-breakpad/src/client/windows/Release/lib -lcommon
    }
}
linux(): LIBS += -L$$PWD/../../build-breakpad/src/client/linux -lbreakpad_client
# END breakpad


folder_01.source = ./icons
folder_01.target = .
DEPLOYMENTFOLDERS = folder_01

# BEGIN Create make target 'copyResourceFolders'
copyCommand = @echo Copying application data ...
!isEqual(PWD,$$OUT_PWD) {
    for(deploymentfolder, DEPLOYMENTFOLDERS) {
        source = $$PWD/$$eval($${deploymentfolder}.source)
        sourcePathSegments = $$split(source, /)

        windows() {
            CONFIG(debug, debug|release): MODE = debug
            CONFIG(release, debug|release): MODE = release
            target = $$OUT_PWD/$$MODE/$$eval($${deploymentfolder}.target)/$$last(sourcePathSegments)
        }
        osx() {
            target = $$OUT_PWD/$${TARGET}.app/Contents/Resources/$$eval($${deploymentfolder}.target)
        }
        linux() {
            target = $$OUT_PWD/$$eval($${deploymentfolder}.target)
        }

        source = $$replace(source, /./, /) # Remove /./ parts
        target = $$replace(target, /./, /) # Remove /./ parts
        source = $$replace(source, //, /) # Remove consecutive slashes
        target = $$replace(target, //, /) # Remove consecutive slashes

        windows {
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
copyResourceFolders.commands = $$copyCommand
# END Create make target 'copyResourceFolders'


# Ensure that copy targets are made (at some random point in time)
QMAKE_EXTRA_TARGETS += all
QMAKE_EXTRA_TARGETS += copyLicenses
QMAKE_EXTRA_TARGETS += copyResourceFolders
QMAKE_EXTRA_TARGETS += copyTestUi
all.depends += copyLicenses
all.depends += copyResourceFolders
all.depends += copyTestUi

include(../common_post.pri)
