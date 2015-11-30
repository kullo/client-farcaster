#!/bin/bash
set -e
which shellcheck > /dev/null && shellcheck "$0"

if [[ $(uname) == "Darwin" ]]; then
    QT_DIR="$HOME/.qt/"
else
    QT_DIR="/opt/qt"
fi

# JavaScript file check temporary disabled due to
# https://bugreports.qt.io/browse/QTBUG-45916
# Reactivate using the following find pattern: ".*\.\(qml\|js\)"

find ./kullodesktop/ -type f -regex ".*\.\(qml\)" -exec "$QT_DIR/bin/qmllint" \{\} +
