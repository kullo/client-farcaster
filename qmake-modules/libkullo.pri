INCLUDEPATH += $$LIBKULLO_BIN_DIR/include
LIBS += -L$$LIBKULLO_BIN_DIR/lib \
    -lkulloclient \
    -largon2 \
    -ljsoncpp \
    -lgmock \
    -lsmartsqlite \
    -lboost_program_options \
    -lz \
    -lboost_filesystem \
    -lboost_system

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
    $${TD_PREFIX}argon2$${TD_SUFFIX} \
    $${TD_PREFIX}jsoncpp$${TD_SUFFIX} \
    $${TD_PREFIX}gmock$${TD_SUFFIX} \
    $${TD_PREFIX}smartsqlite$${TD_SUFFIX} \
    $${TD_PREFIX}boost_program_options$${TD_SUFFIX} \
    $${TD_PREFIX}z$${TD_SUFFIX} \
    $${TD_PREFIX}boost_filesystem$${TD_SUFFIX} \
    $${TD_PREFIX}boost_system$${TD_SUFFIX}

windows(): {
    PRE_TARGETDEPS += $${TD_PREFIX}boost_regex$${TD_SUFFIX}
    LIBS += -lboost_regex
}
