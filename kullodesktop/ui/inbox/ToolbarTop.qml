/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Controls 1.3

import "../buttons"
import "../formelements"
import "../js/shortcut.js" as SC

Rectangle {
    height: theButtonsRow.implicitHeight
            + 2*theButtonsRow.anchors.topMargin
            + borderbottom.height
    color: "#E6E6E6"

    Row {
        id: theButtonsRow
        anchors {
            left: parent.left
            leftMargin: theButtonsRow.spacing
            right: parent.right
            rightMargin: theButtonsRow.spacing
            top: parent.top
            topMargin: 5
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
