include(../common_pre.pri)

# SRC_DIR and OUT_DIR are analogues to PWD and OUT_PWD
# but use backslashes on Windows
# This must not be done in .pri files due to different values of PWD
SRC_DIR=$$shell_path($$PWD)
OUT_DIR=$$shell_path($$OUT_PWD)

QT = core gui qml quick widgets \
    core-private gui-private \
    multimedia printsupport

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
    copyOpenSslCommand += ; rsync -pgo --update \"$$OUT_PWD/../../build-openssl/lib/libcrypto.so.1.0.0\" \"$$OUT_PWD/../../build-openssl/lib/libssl.so.1.0.0\" \"$$OUT_PWD/\"
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
    imageproviders/messageavatarprovider.cpp \
    imageproviders/participantavatarprovider.cpp \
    imageproviders/senderavatarprovider.cpp \
    imageproviders/usersettingsavatarprovider.cpp \
    osintegration/kullotrayicon.cpp \
    osintegration/programoptions.cpp \
    osintegration/singleinstanceerrorbox.cpp \
    osintegration/singleinstancelock.cpp \
    qml/attachmentlistmodel.cpp \
    qml/attachmentmodel.cpp \
    qml/attachmentsaddermodel.cpp \
    qml/changelog.cpp \
    qml/clientmodel.cpp \
    qml/conversationlistmodel.cpp \
    qml/conversationlistsource.cpp \
    qml/conversationmodel.cpp \
    qml/devicesettings.cpp \
    qml/draftattachmentlistmodel.cpp \
    qml/draftattachmentmodel.cpp \
    qml/draftmodel.cpp \
    qml/existencechecker.cpp \
    qml/fileopendialog.cpp \
    qml/filesavedialog.cpp \
    qml/fontlist.cpp \
    qml/kulloupdatermodel.cpp \
    qml/kulloversionchecker.cpp \
    qml/libraryloggermodel.cpp \
    qml/loginchecker.cpp \
    qml/messagemodel.cpp \
    qml/messagelistmodel.cpp \
    qml/messagelistsource.cpp \
    qml/modalitywatcher.cpp \
    qml/participantmodel.cpp \
    qml/registerer.cpp \
    qml/scheduler.cpp \
    qml/settingslocationmodel.cpp \
    qml/softwareversions.cpp \
    qml/usersettingsmodel.cpp \
    qml/utils.cpp \
    util/breakpadsetup.cpp \
    util/kullofoldersetup.cpp \
    util/logengineconsoleextended.cpp \
    util/logenginefilewriter.cpp \
    util/logfilecleaner.cpp \
    util/paths.cpp \
    util/qdebugmessagehandler.cpp \
    util/qmlsetup.cpp \
    visuals/advancedrectangle.cpp \
    visuals/hint.cpp \
    main.cpp

HEADERS += \
    applications/exceptionawareqapplication.h \
    applications/kulloapplication.h \
    applications/osxkulloapplication.h \
    imageproviders/abstractavatarprovider.h \
    imageproviders/attachmentpreviewprovider.h \
    imageproviders/conversationavatarprovider.h \
    imageproviders/messageavatarprovider.h \
    imageproviders/participantavatarprovider.h \
    imageproviders/senderavatarprovider.h \
    imageproviders/usersettingsavatarprovider.h \
    osintegration/kullotrayicon.h \
    osintegration/programoptions.h \
    osintegration/singleinstanceerrorbox.h \
    osintegration/singleinstancelock.h \
    qml/attachmentmodel.h \
    qml/attachmentlistmodel.h \
    qml/attachmentsaddermodel.h \
    qml/changelog.h \
    qml/clientmodel.h \
    qml/conversationlistmodel.h \
    qml/conversationlistsource.h \
    qml/conversationmodel.h \
    qml/devicesettings.h \
    qml/draftattachmentlistmodel.h \
    qml/draftattachmentmodel.h \
    qml/draftmodel.h \
    qml/existencechecker.h \
    qml/fileopendialog.h \
    qml/filesavedialog.h \
    qml/fontlist.h \
    qml/kulloupdatermodel.h \
    qml/kulloversionchecker.h \
    qml/libraryloggermodel.h \
    qml/loginchecker.h \
    qml/messagemodel.h \
    qml/messagelistmodel.h \
    qml/messagelistsource.h \
    qml/modalitywatcher.h \
    qml/os.h \
    qml/participantmodel.h \
    qml/registerer.h \
    qml/scheduler.h \
    qml/settingslocationmodel.h \
    qml/softwareversions.h \
    qml/usersettingsmodel.h \
    qml/utils.h \
    util/breakpadsetup.h \
    util/kullofoldersetup.h \
    util/logengineconsoleextended.h \
    util/logenginefilewriter.h \
    util/logfilecleaner.h \
    util/paths.h \
    util/qdebugmessagehandler.h \
    util/qmlsetup.h \
    visuals/advancedrectangle.h \
    visuals/hint.h \
    farcaster-forwards.h

# BEGIN apimirror
win32:CONFIG(release, debug|release):     LIBS += -L$$OUT_PWD/../apimirror/release/ -lapimirror
else:win32:CONFIG(debug, debug|release):  LIBS += -L$$OUT_PWD/../apimirror/debug/   -lapimirror
else:unix:                                LIBS += -L$$OUT_PWD/../apimirror/         -lapimirror

win32:CONFIG(release, debug|release):     PRE_TARGETDEPS += $$OUT_PWD/../apimirror/release/apimirror.lib
else:win32:CONFIG(debug, debug|release):  PRE_TARGETDEPS += $$OUT_PWD/../apimirror/debug/apimirror.lib
unix:                                     PRE_TARGETDEPS += $$OUT_PWD/../apimirror/libapimirror.a
# END apimirror

# BEGIN cpp-markdown
win32:CONFIG(release, debug|release):     LIBS += -L$$OUT_PWD/../cpp-markdown/release/ -lcpp-markdown
else:win32:CONFIG(debug, debug|release):  LIBS += -L$$OUT_PWD/../cpp-markdown/debug/   -lcpp-markdown
else:unix:                                LIBS += -L$$OUT_PWD/../cpp-markdown/         -lcpp-markdown

DEPENDPATH += $$PWD/../cpp-markdown

win32:CONFIG(release, debug|release):    PRE_TARGETDEPS += $$OUT_PWD/../cpp-markdown/release/cpp-markdown.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../cpp-markdown/debug/cpp-markdown.lib
else:unix:                               PRE_TARGETDEPS += $$OUT_PWD/../cpp-markdown/libcpp-markdown.a
# END cpp-markdown

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

# BEGIN breakpadwrapper
windows() | linux() {
    windows(): LIBS += -lshell32 -lole32
	LIBS += -lbreakpadwrapper
}
# END breakpadwrapper

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

# BEGIN google-breakpad
win32 {
    CONFIG(debug, debug|release) {
        LIBS += -L$$PWD/../../google-breakpad/breakpad-code/src/client/windows/handler/Debug/lib -lexception_handler
        LIBS += -L$$PWD/../../google-breakpad/breakpad-code/src/client/windows/crash_generation/Debug/lib -lcrash_generation_client
        LIBS += -L$$PWD/../../google-breakpad/breakpad-code/src/client/windows/Debug/lib -lcommon
    }
    CONFIG(release, debug|release) {
        LIBS += -L$$PWD/../../google-breakpad/breakpad-code/src/client/windows/handler/Release/lib -lexception_handler
        LIBS += -L$$PWD/../../google-breakpad/breakpad-code/src/client/windows/crash_generation/Release/lib -lcrash_generation_client
        LIBS += -L$$PWD/../../google-breakpad/breakpad-code/src/client/windows/Release/lib -lcommon
    }
}
linux(): LIBS += -L$$PWD/../../google-breakpad/breakpad-code/src/client/linux -lbreakpad_client
# END google-breakpad


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
