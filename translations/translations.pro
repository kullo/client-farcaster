TEMPLATE = aux

CONFIG -= qt

# relative to this .pro file in source repository
TRANSLATIONS_DIR = .

TRANSLATIONS += \
    $$TRANSLATIONS_DIR/farcaster_de.ts

!isEmpty(TRANSLATIONS) {
    isEmpty(QMAKE_LRELEASE) {
        win32:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]\lrelease.exe
        else:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
    }

    isEmpty(TRANSLATIONS_DIR) {
        error("TRANSLATIONS_DIR not set.")
    }

    # Compile .qm files into the same directory as .ts files
    updateqm.name = lrelease ${QMAKE_FILE_IN}
    updateqm.input = TRANSLATIONS
    updateqm.output = $$PWD/$$TRANSLATIONS_DIR/${QMAKE_FILE_BASE}.qm
    updateqm.commands = $$QMAKE_LRELEASE ${QMAKE_FILE_IN}
    updateqm.CONFIG = no_link no_clean
    QMAKE_EXTRA_COMPILERS += updateqm
}
else {
    error(No translation files in project)
}

PRE_TARGETDEPS += compiler_updateqm_make_all
