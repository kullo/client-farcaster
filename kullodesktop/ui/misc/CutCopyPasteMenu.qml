/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Controls 1.3

MouseArea {
    id: _root

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
    onClicked: _menu.popup()

    Menu {
        id: _menu
        enabled: _root.enabled

        MenuItem {
            visible: hasCut
            text: qsTr("Cut")
            onTriggered: _root.cut()
        }

        MenuItem {
            visible: hasCopy
            text: qsTr("Copy")
            onTriggered: _root.copy()
        }
        MenuItem {
            visible: hasPaste
            text: qsTr("Paste")
            onTriggered: _root.paste()
        }

        MenuSeparator {
            visible: hasSelectAll
        }
        MenuItem {
            visible: hasSelectAll
            text: qsTr("Select all")
            onTriggered: _root.selectAll()
        }
    }
}
