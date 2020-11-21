/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
import QtQuick 2.6

import ".."
import "../native"

Column {
    /* public */
    property var conversationParticipantsAddresses: [] // stringlist
    property int paddingTop: 10 * Hdpi.FontScalingFactor
    property int paddingRight: 10 * Hdpi.FontScalingFactor
    property int paddingBottom: 10 * Hdpi.FontScalingFactor
    property int paddingLeft: 10 * Hdpi.FontScalingFactor

    id: root

    anchors {
        left: parent.left
        leftMargin: paddingLeft
        right: parent.right
        rightMargin: paddingRight
    }

    height: implicitHeight + paddingBottom

    topPadding: paddingTop
    spacing: 3 * Hdpi.FontScalingFactor

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
