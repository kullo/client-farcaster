include(../common_pre.pri)

TARGET = cpp-markdown
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    markdown.cpp \
    markdown-tokens.cpp

HEADERS += \
    markdown.h \
    markdown-tokens.h

*-g++ {
    QMAKE_CXXFLAGS_WARN_ON += \
        -Wno-unused-parameter \
        -Wno-unused-function \
        -Wno-unused-variable
}

include(../common_post.pri)
