CURLWRAPPER_ROOT = $$CMAKE_DEPENDENCIES_ROOT/bin-curlwrapper

INCLUDEPATH += $$CURLWRAPPER_ROOT/include

win32 {
    TD_FILE_CURL = libcurl.lib
    TD_FILE_CURLCPP = curlcpp.lib
} else {
    TD_FILE_CURL = libcurl.a
    TD_FILE_CURLCPP = libcurlcpp.a
}
PRE_TARGETDEPS += \
    $$CURLWRAPPER_ROOT/lib/$${TD_FILE_CURL} \
    $$CURLWRAPPER_ROOT/lib/$${TD_FILE_CURLCPP}

win32: QMAKE_CXX_FLAGS += -DCURL_STATICLIB

win32:     LIBS += -L$$CURLWRAPPER_ROOT/lib -lcurlcpp -llibcurl -lws2_32 -lcrypt32
else:macx: LIBS += -L$$CURLWRAPPER_ROOT/lib -lcurlcpp -lcurl -framework Security -framework Foundation -lz
else:      LIBS += -L$$CURLWRAPPER_ROOT/lib -lcurlcpp -lcurl -lz

linuxRelease(): LIBS += -L$$SOURCE_ROOT/../build-linux-x86_64-release/build-openssl/lib -lssl -lcrypto
linuxDebug():   LIBS += -L$$SOURCE_ROOT/../build-linux-x86_64-debug/build-openssl/lib -lssl -lcrypto
