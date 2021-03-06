include(common_pre.pri)

TEMPLATE = subdirs

#
# (crashreporter) (tests)  (kullodesktop)
#         \         |       /     |
#          \        |      /      |
#           \       |     /       |
#            \      |    /        |
#             \     |   /         |
#            (desktoputil)  (apimirror)
#                 /\             /  \
#                /  \           /    \           internal
# - - - - - - - / - -\- - - - -/- - - \ - - - - - - - - -
#              /      \       /        \         external
#         (libkullo)  (Qt)  (libkullo) (Qt)
#             |                 |
#             |                 |
#          (boost)           (boost)
#

SUBDIRS += \
    apimirror \
    desktoputil \
    integration-tests \
    kullodesktop \
    tests \
    translations

!macx {
    SUBDIRS += crashreporter
    crashreporter.depends += desktoputil
}

linux() {
    SUBDIRS += registerer
    registerer.depends += desktoputil
}

tests.depends += desktoputil
integration-tests.depends += desktoputil

kullodesktop.depends += apimirror
kullodesktop.depends += desktoputil
kullodesktop.depends += translations

include(common_post.pri)
