/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
import QtQuick 2.4
import QtQuick.Controls.Private 1.0
import Kullo 1.0

// TooltipArea.qml
// This file contains private Qt Quick modules that might change in future versions of Qt
// Tested on: Qt 5.4.1 - Qt 5.6.1

MouseArea {
    property string text: ""

    id: root
    anchors.fill: parent
    acceptedButtons: Qt.NoButton
    hoverEnabled: root.enabled

    onExited: Tooltip.hideText()
    onCanceled: Tooltip.hideText()

    StableTimer {
        interval: 1000
        running: root.enabled && root.containsMouse && root.text.length
        onTriggered: Tooltip.showText(root, Qt.point(root.mouseX, root.mouseY), root.text)
    }
}
