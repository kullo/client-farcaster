/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Controls 1.3

import "../"
import "../buttons"
import "../formelements"
import "../js/shortcut.js" as SC

Rectangle {
    height: theButtonsRowRight.implicitHeight
            + 2*theButtonsRowRight.anchors.topMargin
            + borderbottom.height
    color: Style.leftToolbarBackground

    Row {
        id: theButtonsRowLeft
        anchors {
            left: parent.left
            leftMargin: theButtonsRowLeft.spacing
            top: parent.top
            topMargin: 5 + 2 // the FSubmitButton is 4px smaller than the IconButton, thus add 2
        }
        spacing: 5

        FForm {
            onAccepted: inboxScreen.startConversation()
            height: startConversationButton.implicitHeight
            width: startConversationButton.implicitWidth

            FSubmitButton {
                id: startConversationButton
                style: KulloButtonStyle {
                    source: "/resources/scalable/add_w.svg"
                    iconPosition: _POSITION_LEFT
                }
                text: qsTr("Add")
                tooltip: qsTr("Create a new conversation")
                         + " (%1)".arg(SC.nameOfCtrlAndKey("N", Os.osx))
            }
        }
    }

    Row {
        id: theButtonsRowRight
        anchors {
            right: parent.right
            rightMargin: theButtonsRowRight.spacing
            top: parent.top
            topMargin: 5
        }
        spacing: 5

        IconButton {
            source: "/resources/scalable/search_b.svg"
            tooltip: qsTr("Search")
            onClicked: inboxScreen.openSearch()
        }

        IconButton {
            source: inboxScreen.todoMode
                    ? "/resources/scalable/todo_active_b.svg"
                    : "/resources/scalable/todo_b.svg"
            onClicked: inboxScreen.toggleTodoMode()
            tooltip: (inboxScreen.todoMode ? qsTr("Switch todo mode off") : qsTr("Switch todo mode on"))
                     + " (%1)".arg(SC.nameOfCtrlAndKey("T", Os.osx))
        }
    }

    Rectangle {
        id: borderbottom
        color: "#ccc"
        height: 0
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
            leftMargin: 3
            rightMargin: 3
        }
    }
}
