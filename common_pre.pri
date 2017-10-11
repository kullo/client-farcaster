CONFIG += c++11

# Disable GNU extensions (default for Qt 5.5 and older)
# See ff4444065158285b573c6fc0a91c9e75fc7ddd96
# https://codereview.qt-project.org/#/c/121023/
CONFIG += strict_c++

CONFIG(release, debug|release): DEFINES += NDEBUG

defineTest(osx) {
    macx: return(true)
    return(false)
}

defineTest(osxDebug) {
    osx():CONFIG(debug, debug|release): return(true)
    return(false)
}

defineTest(osxRelease) {
    osx():CONFIG(release, debug|release): return(true)
    return(false)
}

defineTest(windows) {
    win32: return(true)
    return(false)
}

defineTest(windowsDebug) {
    windows():CONFIG(debug, debug|release): return(true)
    return(false)
}

defineTest(windowsRelease) {
    windows():CONFIG(release, debug|release): return(true)
    return(false)
}

defineTest(linux) {
    unix:!mac: return(true)
    return(false)
}

defineTest(linuxDebug) {
    linux():CONFIG(debug, debug|release): return(true)
    return(false)
}

defineTest(linuxRelease) {
    linux():CONFIG(release, debug|release): return(true)
    return(false)
}

# For $$QT_ARCH see https://bugreports.qt.io/browse/QTBUG-30263
contains(QT_ARCH, x86_64) {
    ARCH = x86_64
} else {
    ARCH = x86_32
}

windowsRelease():    CMAKE_DEPENDENCIES_ROOT = $$BUILD_ROOT\..\build-windows-$$ARCH-release
else:windowsDebug(): CMAKE_DEPENDENCIES_ROOT = $$BUILD_ROOT\..\build-windows-$$ARCH-debug
else:osxRelease():   CMAKE_DEPENDENCIES_ROOT = $$BUILD_ROOT/../build-osx-$$ARCH-release
else:osxDebug():     CMAKE_DEPENDENCIES_ROOT = $$BUILD_ROOT/../build-osx-$$ARCH-debug
else:linuxRelease(): CMAKE_DEPENDENCIES_ROOT = $$BUILD_ROOT/../build-linux-$$ARCH-release
else:linuxDebug():   CMAKE_DEPENDENCIES_ROOT = $$BUILD_ROOT/../build-linux-$$ARCH-debug
else: error("Unknown os")

LIBKULLO_BIN_DIR = $$CMAKE_DEPENDENCIES_ROOT/bin-libkullo

# Add farcaster repo root and libkullo include dir to include path
INCLUDEPATH += $$PWD
INCLUDEPATH += $$LIBKULLO_BIN_DIR/include
DEFINES += BOOST_ALL_NO_LIB

macx {
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.10
}

# Hardening
linuxRelease(): {
    QMAKE_CXXFLAGS += -fstack-protector -fPIE -D_FORTIFY_SOURCE=2
    QMAKE_LFLAGS += -pie -z relro -z now
}

# Breakpad
*-g++:CONFIG(release, debug|release) {
    QMAKE_CXXFLAGS += -g
}

# Fix make's $(DEL_DIR)
#
# rmdir:
# /s : Removes the specified directory and all subdirectories including any files. Use /s to remove a tree.
# /q : Runs rmdir in quiet mode. Deletes directories without confirmation.
#
# rm:
#  -f    ignore nonexistent files and arguments, never prompt
#  -r    remove directories and their contents recursively
win32:QMAKE_DEL_DIR = rmdir /s /q
unix:QMAKE_DEL_DIR = rm -rf
