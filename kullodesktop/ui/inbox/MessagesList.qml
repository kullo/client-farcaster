/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
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
        messagesList: root
        highlightColor: root.activeFocus ? "#999999" : "#bbbbbb"

        conversationId: conversationId_
        messageId: id_
        textAsHtml: textAsHtml_
        footer: footer_
        senderAddress: senderAddress_
        senderName: senderName_
        senderOrganization: senderOrganization_
        read: read_
        done: done_
        deliveryStatus: deliveryStatus_
        incoming: incoming_
        dateReceived: dateReceived_
        attachments: attachments_
        attachmentsReady: attachmentsReady_

        anchors {
            left: parent.left
            right: parent.right
        }

        onAvatarDoubleClicked: {
            senderInfoWindow.conversationId = conversationId_
            senderInfoWindow.messageId = id_
            senderInfoWindow.senderName = senderName_
            senderInfoWindow.senderOrganization = senderOrganization_
            senderInfoWindow.senderAddress = senderAddress_
            senderInfoWindow.openWindow()
        }

        onQuoteRequested: {
            inboxScreen.openAnswer()
            rightColumn.addQuoteToAnswer(quoteText)
        }

        Component.onCompleted: {
            saveAttachmentRequested.connect(inboxScreen.openSaveAttachmentDialog)
            saveAttachmentsRequested.connect(inboxScreen.openSaveAttachmentsDialog)
        }
    }

    footer: ConversationIntro {
        conversationParticipantsAddresses: participantsAddresses
        paddingTop: 5 * Hdpi.FontScalingFactor
        paddingRight: 5 * Hdpi.FontScalingFactor
        paddingBottom: 10 * Hdpi.FontScalingFactor
        paddingLeft: 5 * Hdpi.FontScalingFactor
    }

    cacheBuffer: 1000 // px
}
