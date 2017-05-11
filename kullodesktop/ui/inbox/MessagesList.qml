/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import Kullo 1.0

import "../windows"

ListView {
    id: root
    anchors {
        left: parent.left
        right: parent.right
        top: parent.top
        bottom: parent.bottom
    }
    clip: true

    interactive: false

    function scrollUp()       { scroll(-100) }
    function scrollDown ()    { scroll(+100) }
    function scrollPageUp()   { scroll(-(height-50)) }
    function scrollPageDown() { scroll(+(height-50)) }

    function scrollTop()
    {
        var newIndex = 0
        positionViewAtIndex(newIndex, ListView.Beginning)
        currentIndex = newIndex
    }

    function scrollBottom()   {
        var newIndex = count-1
        positionViewAtIndex(newIndex, ListView.End)
        currentIndex = newIndex
    }

    function scroll(pixels) {
        var newContentY = contentY + pixels
        newContentY = Math.max(newContentY, 0);
        contentY = newContentY
    }

    StableTimer {
        id: deleteTimer
        property int currentlyDeletionMessageId: -1
        repeat: false
        onTriggered: {
            root.model.deleteMessage(currentlyDeletionMessageId)
            currentlyDeletionMessageId = -1
        }
    }

    // Calling this function makes deletion independent of the lifetime of the delegate
    function triggerDelayedDeletion(messageId, timeout) {
        if (deleteTimer.currentlyDeletionMessageId != -1) return false // in deletion process

        deleteTimer.interval = timeout
        deleteTimer.currentlyDeletionMessageId = messageId
        deleteTimer.start()
        return true
    }

    Keys.onSpacePressed: {
        if (event.modifiers & Qt.ShiftModifier) scrollPageUp()
        else                                    scrollPageDown()
        event.accepted = true
    }

    Keys.onPressed: {
        if (event.modifiers !== 0) return;

        if (event.key === Qt.Key_Up ) {
            scrollUp();
        }
        else if (event.key === Qt.Key_Down ) {
            scrollDown();
        }
        else if (event.key === Qt.Key_PageUp) {
            scrollPageUp()
        }
        else if (event.key === Qt.Key_PageDown) {
            scrollPageDown()
        }
        else if (event.key === Qt.Key_Home) {
            scrollTop()
        }
        else if (event.key === Qt.Key_End) {
            scrollBottom()
        }
        else {
            return;
        }

        // one of the navigation keys has been pressed
        event.accepted = true
    }

    SenderInfoWindow {
        id: senderInfoWindow
    }

    delegate: MessageDelegate {
        anchors {
            left: parent.left
            right: parent.right
        }

        messagesList: root
        highlightColor: root.activeFocus ? "#999999" : "#bbbbbb"

        onAvatarDoubleClicked: {
            senderInfoWindow.conversationId = conversationId_
            senderInfoWindow.messageId = id_
            senderInfoWindow.senderName = senderName_
            senderInfoWindow.senderOrganization = senderOrganization_
            senderInfoWindow.senderAddress = senderAddress_
            senderInfoWindow.openWindow()
        }
    }

    footer: ConversationIntro {
        conversationParticipantsAddresses: participantsAddresses
        paddingTop: 5
        paddingRight: 5
        paddingBottom: 10
        paddingLeft: 5
    }

    cacheBuffer: 1000 // px
}
