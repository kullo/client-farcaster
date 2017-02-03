/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
import QtQuick 2.4

import ".."
import "../native"

Column {
    /* public */
    property var conversationParticipantsAddresses: [] // stringlist
    property int paddingTop: 10
    property int paddingRight: 10
    property int paddingBottom: 10
    property int paddingLeft: 10

    id: root

    anchors {
        left: parent.left
        leftMargin: paddingLeft
        right: parent.right
        rightMargin: paddingRight
    }

    height: implicitHeight + paddingBottom

    spacing: 3

    // Replace this top spacing element by Column.topPadding
    // as soon as Qt 5.6 is available
    Item {
        height: root.paddingTop - root.spacing
        anchors {
            left: parent.left
            right: parent.right
        }
    }

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
