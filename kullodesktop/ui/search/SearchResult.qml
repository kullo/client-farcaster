/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
import QtQuick 2.4

import ".."
import "../native"
import "../js/format.js" as Format

Rectangle {
    /* public */
    width: 400
    property int messageId: -1
    property int conversationId: -1
    property string senderDisplayName: ""
    property string snippet: ""
    property date date
    property bool hasAttachments: false

    signal showMessageInInboxRequested()

    /* private */
    id: root
    property int _VERTICAL_PADDING: 12
    property int _HORIZONTAL_PADDING: 8
    implicitHeight: _VERTICAL_PADDING
                    + header.implicitHeight
                    + 5
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
        height: 40
        width: 40
        asynchronous: true
        source: "image://messagesenderavatars/" + Utils.urlencode(Inbox.userSettings.address)
                + "/" + conversationId + "/" + messageId
    }

    Item {
        id: header
        width: 400 - (2*_HORIZONTAL_PADDING + 40 + 5)
        anchors {
            top: parent.top
            topMargin: _VERTICAL_PADDING
            left: parent.left
            leftMargin: _HORIZONTAL_PADDING + 40 + 5
        }
        implicitHeight: sender.implicitHeight
            + 3
        property color headerTextColor: Style.gray

        NativeText {
            id: sender
            anchors {
                top: parent.top
                left: parent.left
            }
            width: 280
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
            width: 100
            wrapMode: Text.NoWrap
            horizontalAlignment: Text.AlignRight
            text: root.date ? Format.humanDatetime(root.date, Locale.ShortFormat, Locale.ShortFormat) : ""
            color: header.headerTextColor
        }

        Rectangle {
            color: Utils.setAlpha(header.headerTextColor, 0.5)
            height: 1
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
            topMargin: 5
            left: parent.left
            leftMargin: _HORIZONTAL_PADDING + 40 + 5
        }
        width: 400- (_HORIZONTAL_PADDING + 40 + 5) - (meta.width + _HORIZONTAL_PADDING)
        textFormat: Text.StyledText
        text: root.snippet
    }

    Column {
        id: meta
        anchors {
            top: header.bottom
            topMargin: 5
            right: parent.right
            rightMargin: _HORIZONTAL_PADDING
        }
        width: 30

        NativeImage {
            source: "/resources/scalable/attach_b.svg"
            width: 20
            height: 20
            x: 10
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
