windowsDebug(): INCLUDEPATH += $$BUILD_ROOT/../build-curl-debug/include
else:           INCLUDEPATH += $$BUILD_ROOT/../build-curl/include

windowsDebug(): TD_BUILD_DIR = $$BUILD_ROOT/../build-curl-debug
else:           TD_BUILD_DIR = $$BUILD_ROOT/../build-curl
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

win32:CONFIG(release, debug|release):    LIBS += -L$$BUILD_ROOT/../build-curl/lib -lcurlcpp -llibcurl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$BUILD_ROOT/../build-curl-debug/lib -lcurlcpp -llibcurl
else:macx:                               LIBS += -L$$BUILD_ROOT/../build-curl/lib -lcurlcpp -lcurl -framework Security -framework Foundation -lz
else:                                    LIBS += -L$$BUILD_ROOT/../build-curl/lib -lcurlcpp -lcurl -lz

linux():                                 LIBS += -L$$BUILD_ROOT/../build-openssl/lib -lssl -lcrypto
