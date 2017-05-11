/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Controls 1.4

import ".."
import "../native"

Rectangle {
    property string address: ""
    signal openRequested()
    signal logoutRequested()
    signal selectRequested()
    signal aboutToShowContextMenu()
    signal aboutToHideContextMenu()

    id: root

    color: !accountsListView.contextMenuVisible && mouseArea.containsMouse
           ? Style.welcomeAccountsHighlightedBackground
           : "transparent"

    NativeText {
        id: textView
        text: root.address
        anchors {
            left: parent.left
            leftMargin: 10
            right: parent.right
            rightMargin: 5
            verticalCenter: parent.verticalCenter
        }
        color: !accountsListView.contextMenuVisible && mouseArea.containsMouse
               ? Style.welcomeAccountsHighlightedForeground
               : Style.welcomeAccountsForeground
    }

    Menu {
        id: contextMenu

        onAboutToShow: root.aboutToShowContextMenu()
        onAboutToHide: root.aboutToHideContextMenu()

        MenuItem {
            text: qsTr("Open inbox")
            onTriggered: { root.openRequested() }
        }

        MenuSeparator {
        }

        MenuItem {
            text: qsTr("Log out")
            onTriggered: { root.logoutRequested() }
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        onClicked: {
            root.selectRequested()
            if (mouse.button === Qt.LeftButton)
            {
                root.openRequested()
            }
            else if (mouse.button === Qt.RightButton)
            {
                contextMenu.popup()
            }
            else
            {
                console.error("Unhandled mouse click event")
            }
        }
    }
}
