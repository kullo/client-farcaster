/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
import QtQuick 2.4

import ".."
import "../native"
import "../js/format.js" as Format

Rectangle {
    /* public */
    width: 400 * Hdpi.FontScalingFactor
    property int messageId: -1
    property int conversationId: -1
    property string senderDisplayName: ""
    property string snippet: ""
    property date date
    property bool hasAttachments: false

    signal showMessageInInboxRequested()

    /* private */
    id: root
    property int _VERTICAL_PADDING: 12 * Hdpi.FontScalingFactor
    property int _HORIZONTAL_PADDING: 8 * Hdpi.FontScalingFactor
    implicitHeight: _VERTICAL_PADDING
                    + header.implicitHeight
                    + 5 * Hdpi.FontScalingFactor
                    + Math.max(snippet.implicitHeight, meta.implicitHeight)
                    + _VERTICAL_PADDING

    // Do not use ListView's highlight property because
    // height resizing animation is annoying
    color: index === searchResults.currentIndex
           ? "#ddd"
           : "transparent"

    NativeImage {
        id: senderAvatar
        anchors {
            left: parent.left
            leftMargin: _HORIZONTAL_PADDING
            top: parent.top
            topMargin: _VERTICAL_PADDING
        }
        height: 40 * Hdpi.FontScalingFactor
        width: 40 * Hdpi.FontScalingFactor
        asynchronous: true
        source: "image://messagesenderavatars/" + Utils.urlencode(Inbox.userSettings.address)
                + "/" + conversationId + "/" + messageId
    }

    Item {
        id: header
        width: 400 * Hdpi.FontScalingFactor - (2*_HORIZONTAL_PADDING + 45 * Hdpi.FontScalingFactor)
        anchors {
            top: parent.top
            topMargin: _VERTICAL_PADDING
            left: parent.left
            leftMargin: _HORIZONTAL_PADDING + 45 * Hdpi.FontScalingFactor
        }
        implicitHeight: sender.implicitHeight
            + 3 * Hdpi.FontScalingFactor
        property color headerTextColor: Style.gray

        NativeText {
            id: sender
            anchors {
                top: parent.top
                left: parent.left
            }
            width: 280 * Hdpi.FontScalingFactor
            wrapMode: Text.NoWrap
            text: senderDisplayName
            elide: Text.ElideRight
            color: header.headerTextColor
        }

        NativeText {
            id: date
            anchors {
                top: parent.top
                right: parent.right
            }
            width: 100 * Hdpi.FontScalingFactor
            wrapMode: Text.NoWrap
            horizontalAlignment: Text.AlignRight
            text: root.date ? Format.humanDatetime(root.date, Locale.ShortFormat, Locale.ShortFormat) : ""
            color: header.headerTextColor
        }

        Rectangle {
            color: Utils.setAlpha(header.headerTextColor, 0.5)
            height: 1 * Hdpi.FontScalingFactor
            anchors {
                bottom: parent.bottom
                left: parent.left
                right: parent.right
            }
        }
    }

    NativeText {
        id: snippet
        anchors {
            top: header.bottom
            topMargin: 5 * Hdpi.FontScalingFactor
            left: parent.left
            leftMargin: _HORIZONTAL_PADDING + 45 * Hdpi.FontScalingFactor
        }
        width: 400 * Hdpi.FontScalingFactor
               - (_HORIZONTAL_PADDING + 45 * Hdpi.FontScalingFactor)
               - (meta.width + _HORIZONTAL_PADDING)
        textFormat: Text.StyledText
        text: root.snippet
    }

    Column {
        id: meta
        anchors {
            top: header.bottom
            topMargin: 5 * Hdpi.FontScalingFactor
            right: parent.right
            rightMargin: _HORIZONTAL_PADDING
        }
        width: 30 * Hdpi.FontScalingFactor

        NativeImage {
            source: "/resources/scalable/attach_b.svg"
            width: 20 * Hdpi.FontScalingFactor
            height: 20 * Hdpi.FontScalingFactor
            x: 10 * Hdpi.FontScalingFactor
            visible: root.hasAttachments
        }
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton
        onClicked: {
            if (mouse.button === Qt.LeftButton)
            {
                searchResults.currentIndex = index
                searchResults.forceActiveFocus()
            }
        }
        onDoubleClicked: {
            root.showMessageInInboxRequested()
        }
    }
}
