/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
import QtQuick 2.4

import "../buttons"
import "../js/shortcut.js" as SC

Rectangle {
    id: background
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
            source: "/resources/20/logout2_b.png"
            onClicked: inbox.logout()
            tooltip: qsTr("Go to start screen")
                     + " (%1)".arg(SC.nameOfCtrlAndKey("O", Os.osx))
        }

        IconButton {
            source: "/resources/20/settings2_b.png"
            onClicked: inbox.showUserSettingsWindow()
            tooltip: qsTr("Show settings")
                     + " (%1)".arg(SC.nameOfCtrlAndKey("G", Os.osx))
        }

        IconButton {
            source: Devicesettings.muted ? "/resources/20/sound_muted_b.png" : "/resources/20/sound_unmuted_b.png"
            onClicked: inbox.toggleSoundActive()
            tooltip: Devicesettings.muted ? qsTr("Unmute") : qsTr("Mute")
        }

        IconButton {
            source: inbox.todoMode ? "/resources/20/todo_active_b.png" : "/resources/20/todo_b.png"
            onClicked: inbox.toggleTodoMode()
            tooltip: (inbox.todoMode ? qsTr("Switch todo mode off") : qsTr("Switch todo mode on"))
                     + " (%1)".arg(SC.nameOfCtrlAndKey("T", Os.osx))
        }

        IconButton {
            source: "/resources/20/info_b.png"
            onClicked: inbox.showInfoWindow()
            tooltip: qsTr("Show info window")
        }
    }
}
