/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
import QtQuick 2.4

import ".."
import "../native"

Item {
    height: !visible ? 0 :
                       introTextRows.anchors.topMargin
                       + introTextRows.implicitHeight

    Column {
        id: introTextRows
        anchors {
            top: parent.top
            topMargin: 10
            left: parent.left
            right: parent.right
        }
        spacing: 3

        NativeText {
            id: textLine1
            text: qsTr("Started conversation with")
            horizontalAlignment: Text.AlignHCenter
            anchors {
                left: parent.left
                right: parent.right
            }
            font.pointSize: Style.fontSize.conversationIntroNormal
        }
        NativeSelectableText {
            id: textLine2
            text: conversationParticipantsAddresses.join(", ")
            horizontalAlignment: Text.AlignHCenter
            anchors {
                left: parent.left
                right: parent.right
            }
            font.pointSize: Style.fontSize.conversationIntroAdresses
        }
        NativeText {
            id: textLine3
            text: conversationParticipantsAddresses.length === 1 ?
                      qsTr("Please ensure that this is the address you want to write to.")
                    : qsTr("Please ensure that these are the addresses you want to write to.")
            horizontalAlignment: Text.AlignHCenter
            anchors {
                left: parent.left
                right: parent.right
            }
            font.pointSize: Style.fontSize.conversationIntroNormal
        }
    }
}
