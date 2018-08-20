/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.2

import "../native"

NativeModalWindow {
    id: root

    property int contentFooterSpacing: 12
    property int buttonsSpacing: 5
    property int verticalPadding: 12
    property int horizontalPadding: 15

    width: 300
    height: 80
    Component.onCompleted: {
        if (title.trim() === "") console.warn("Dialog created with no title set.")
    }

    signal accepted()
    signal rejected()

    function openDialog() {
        root.openWindow()
    }

    function closeDialog() {
        root.closeWindow()
    }
}
