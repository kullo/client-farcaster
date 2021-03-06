/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.2

import "../native"

NativeModalWindow {
    id: root

    property int contentFooterSpacing: 12 * Hdpi.FontScalingFactor
    property int buttonsSpacing: 5 * Hdpi.FontScalingFactor
    property int verticalPadding: 12 * Hdpi.FontScalingFactor
    property int horizontalPadding: 15 * Hdpi.FontScalingFactor

    width: 300 * Hdpi.FontScalingFactor
    height: 80 * Hdpi.FontScalingFactor
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
