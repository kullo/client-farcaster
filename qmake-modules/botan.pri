LIBS += -L$$CMAKE_DEPENDENCIES_ROOT/bin-botan/lib/ -lbotan
DEPENDPATH += $$CMAKE_DEPENDENCIES_ROOT/bin-botan/lib

win32:     PRE_TARGETDEPS += $$CMAKE_DEPENDENCIES_ROOT/bin-botan/lib/botan.lib
else:unix: PRE_TARGETDEPS += $$CMAKE_DEPENDENCIES_ROOT/bin-botan/lib/libbotan.a

INCLUDEPATH += $$CMAKE_DEPENDENCIES_ROOT/bin-botan/include
