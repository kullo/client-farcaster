#!/bin/bash

set -e
shellcheck "$0"

while IFS= read -r -d '' f
do
    echo "Updating $f ..."
    sed --in-place \
        -e 's/^import QtQuick .*/import QtQuick 2.4/' \
        -e 's/^import QtQuick\.Controls .*/import QtQuick.Controls 1.3/' \
        -e 's/^import QtQuick\.Controls\.Styles .*/import QtQuick.Controls.Styles 1.3/' \
        -e 's/^import QtQuick\.Dialogs .*/import QtQuick.Dialogs 1.2/' \
        -e 's/^import QtQuick\.Layouts .*/import QtQuick.Layouts 1.1/' \
        -e 's/^import QtQuick\.Window .*/import QtQuick.Window 2.2/' \
        "$f"
done <   <(find . -name '*.qml' -print0)

