/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
import QtQuick 2.4
import QtQuick.Controls 1.3

MouseArea {
    id: root

    // default: Copy, Paste
    property bool hasCut: false
    property bool hasCopy: true
    property bool hasPaste: true
    property bool hasSelectAll: false

    signal cut()
    signal copy()
    signal paste()
    signal selectAll()

    anchors.fill: parent
    acceptedButtons: Qt.RightButton
    onClicked: menu.popup()

    Menu {
        id: menu
        enabled: root.enabled

        MenuItem {
            visible: hasCut
            text: qsTr("Cut")
            onTriggered: root.cut()
        }

        MenuItem {
            visible: hasCopy
            text: qsTr("Copy")
            onTriggered: root.copy()
        }
        MenuItem {
            visible: hasPaste
            text: qsTr("Paste")
            onTriggered: root.paste()
        }

        MenuSeparator {
            visible: hasSelectAll
        }
        MenuItem {
            visible: hasSelectAll
            text: qsTr("Select all")
            onTriggered: root.selectAll()
        }
    }
}
