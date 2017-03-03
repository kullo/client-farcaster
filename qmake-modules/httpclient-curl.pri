LIBS += -L$$CMAKE_DEPENDENCIES_ROOT/bin-httpclient-curl/lib/ -lhttpclient
DEPENDPATH += $$CMAKE_DEPENDENCIES_ROOT/bin-httpclient-curl/lib

win32:     PRE_TARGETDEPS += $$CMAKE_DEPENDENCIES_ROOT/bin-httpclient-curl/lib/httpclient.lib
else:unix: PRE_TARGETDEPS += $$CMAKE_DEPENDENCIES_ROOT/bin-httpclient-curl/lib/libhttpclient.a

INCLUDEPATH += $$CMAKE_DEPENDENCIES_ROOT/bin-httpclient-curl/include
