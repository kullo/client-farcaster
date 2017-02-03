/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
import QtQuick 2.4

import "../buttons"
import "../js/shortcut.js" as SC

Rectangle {
    height: borderTop.height
            + theContentRow.implicitHeight
            + 2*theContentRow.anchors.topMargin
    color: "#80ffffff"

    Rectangle {
        id: borderTop
        color: "#ccc"
        height: 1
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            leftMargin: 3
            rightMargin: 3
        }
    }

    Row {
        id: theContentRow
        anchors {
            top: borderTop.bottom
            topMargin: 5
            left: parent.left
            leftMargin: 5
            right: parent.right
        }
        spacing: 5

        IconButton {
            source: "/resources/scalable/logout2_b.svg"
            onClicked: inboxScreen.logout()
            tooltip: qsTr("Go to start screen")
                     + " (%1)".arg(SC.nameOfCtrlAndKey("O", Os.osx))
        }

        IconButton {
            source: "/resources/scalable/settings2_b.svg"
            onClicked: inboxScreen.showUserSettingsWindow()
            tooltip: qsTr("Show settings")
                     + " (%1)".arg(SC.nameOfCtrlAndKey("G", Os.osx))
        }

        IconButton {
            source: InnerApplication.deviceSettings.muted
                    ? "/resources/scalable/sound_muted_b.svg"
                    : "/resources/scalable/sound_unmuted_b.svg"
            onClicked: inboxScreen.toggleSoundActive()
            tooltip: InnerApplication.deviceSettings.muted ? qsTr("Unmute") : qsTr("Mute")
        }

        IconButton {
            source: inboxScreen.todoMode
                    ? "/resources/scalable/todo_active_b.svg"
                    : "/resources/scalable/todo_b.svg"
            onClicked: inboxScreen.toggleTodoMode()
            tooltip: (inboxScreen.todoMode ? qsTr("Switch todo mode off") : qsTr("Switch todo mode on"))
                     + " (%1)".arg(SC.nameOfCtrlAndKey("T", Os.osx))
        }

        IconButton {
            source: "/resources/scalable/info_b.svg"
            onClicked: inboxScreen.showInfoWindow()
            tooltip: qsTr("Show info window")
        }
    }
}
