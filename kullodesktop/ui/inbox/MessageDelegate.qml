/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Controls 1.3
import Kullo 1.0

import "../"
import "../buttons"
import "../misc"
import "../native"
import "../js/format.js" as Format
import "../js/shortcut.js" as SC

Item {
    /* public */
    property var messagesList
    property int paddingTop: 5
    property int paddingRight: 5
    property int paddingBottom: 5
    property int paddingLeft: 5
    property int borderWidth: 4
    property int conversationId: -1
    property int messageId: -1
    property string highlightColor: "#bbbbbb"
    property int deleteAnimationDuration: 300 /* ms */
    property string textAsHtml
    property string footer
    property string senderAddress
    property string senderName
    property string senderOrganization
    property bool read
    property bool done
    property string deliveryStatus
    property bool incoming
    property date dateReceived
    property var attachments
    property bool attachmentsReady

    signal avatarClicked()
    signal avatarDoubleClicked()
    signal quoteRequested(var quoteText)
    signal saveAttachmentRequested(var attachment)
    signal saveAttachmentsRequested(var attachments)

    /* private */
    property bool _selected: ListView.isCurrentItem
    property bool _showFooter: false
    property bool _hasFooter: footer.trim() !== ""
    property real _maxTextWidth: 600*Hdpi.FontScalingFactor


    id: root

    height: paddingTop + messageBorder.height + paddingBottom
    clip: true

    states: State {
        name: "deleting"
        PropertyChanges {
            target: root
            opacity: 0
            height: 0
        }
    }

    transitions: Transition {
        NumberAnimation { properties: "opacity,height"; duration: root.deleteAnimationDuration }
    }

    on_SelectedChanged: {
        if (_selected) {
            if (!read_) markAsRead()
        }
    }

    function markAsRead() {
        read_ = true
    }

    function markAsDone() {
        read_ = true
        done_ = true
    }

    function markAsUndone() {
        done_ = false
    }

    function selectThisMessage() {
        if (messagesList) {
            messagesList.currentIndex = index
        }
    }

    // Keep border a separate component because "The border is rendered within the
    // rectangle's boundaries." so offsetting every child would be painful otherwise.
    Rectangle {
        id: messageBorder
        color: "transparent"
        border {
            color: _selected ? highlightColor : "transparent"
            width: root.borderWidth
        }

        y: paddingTop
        anchors {
            left: parent.left
            right: parent.right
            leftMargin: paddingLeft
            rightMargin: paddingRight
        }
        height: messageBackground.height + 2*root.borderWidth

        Rectangle {
            id: messageBackground
            color: Style.messageBackground

            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
                topMargin: root.borderWidth
                leftMargin: root.borderWidth
                rightMargin: root.borderWidth
            }
            height: header.height + body.height + footerContainer.height

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    if (messagesList) {
                        selectThisMessage()
                        messagesList.forceActiveFocus()
                    }
                }
            }

            MessageHeader {
                id: header
                date: root.dateReceived
                showMessageState: root.incoming
                showMessageDeliveryStatus: !root.incoming
                messageDeliveryStatus: root.deliveryStatus
                read: root.read
                done: root.done
                name: root.senderName
                organization: root.senderOrganization
                address: root.senderAddress
                avatarSource: "image://messagesenderavatars/" + Utils.urlencode(Inbox.userSettings.address)
                              + "/" + root.conversationId + "/" + root.messageId

                onAvatarClicked: root.avatarClicked()
                onAvatarDoubleClicked: root.avatarDoubleClicked()
            }

            Item {
                id: body
                anchors {
                    top: header.bottom
                    left: parent.left
                    right: parent.right
                }
                height: messageTextBox.anchors.topMargin
                        + messageTextBox.height
                        + messageTextBox.anchors.bottomMargin
                        + attachmentsBox.height

                NativeSelectableText {
                    id: messageTextBox
                    text: "<div style='white-space: pre-wrap;'>" + textAsHtml + "</div>"
                    anchors {
                        top: parent.top
                        left: parent.left
                        topMargin: 20
                        leftMargin: 15
                        bottomMargin: 20
                    }
                    width: (body.width-2*anchors.leftMargin) < _maxTextWidth
                           ? (body.width-2*anchors.leftMargin)
                           : _maxTextWidth
                    font.family: InnerApplication.deviceSettings.messagesFontPreview.family
                    font.italic: InnerApplication.deviceSettings.messagesFontPreview.italic
                    font.weight: InnerApplication.deviceSettings.messagesFontPreview.weight
                    font.pointSize: Style.fontSize.message
                    color: Style.messageText
                    textFormat: TextEdit.RichText
                    onLinkActivated: {
                        var scheme = Format.scheme_from_url(link)

                        if (scheme === "http" || scheme === "https")
                        {
                            if (!Qt.openUrlExternally(link))
                            {
                                console.warn("Could not open url: " + link)
                            }
                        }
                        else if (scheme === "kulloInternal") {
                            var address = link.substring(scheme.length+1)
                            startConversationPrompt.openDialog()
                            startConversationPrompt.inputText = address
                        } else {
                            console.warn("Unrecognized sheme: " + scheme)
                        }
                    }

                    function quoteSelectedText() {
                        var quoteText = messageTextBox.selectedText.trim()
                        root.quoteRequested(quoteText)
                    }

                    Menu {
                        id: contextMenu

                        MenuItem {
                            text: qsTr("Copy")
                                  + " (%1)".arg(SC.nameOfCtrlAndKey("C", Os.osx))
                            onTriggered: messageTextBox.copy()
                        }
                        MenuItem {
                            text: qsTr("Quote")
                                  + " (%1)".arg(SC.nameOfCtrlAndKey("B", Os.osx))
                            onTriggered: messageTextBox.quoteSelectedText()
                        }
                    }

                    Menu {
                        id: webLinkMenu
                        property string link: ""

                        MenuItem {
                            text: qsTr("Copy link address")
                            onTriggered: Utils.setClipboardText(webLinkMenu.link)
                        }
                    }

                    Menu {
                        id: kulloAddressLinkMenu
                        property string address: ""

                        MenuItem {
                            text: qsTr("Copy Kullo address")
                            onTriggered: Utils.setClipboardText(kulloAddressLinkMenu.address)
                        }
                    }

                    Keys.onPressed: {
                        // Ctrl+B
                        if (SC.isCtrlAndKey(Qt.Key_B, event))
                        {
                            quoteSelectedText()
                            event.accepted = true
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        acceptedButtons: Qt.RightButton
                        onClicked: {
                            if (messageTextBox.selectionStart != messageTextBox.selectionEnd)
                            {
                                contextMenu.popup()
                            }
                            else
                            {
                                var link = messageTextBox.linkAt(mouseX, mouseY)
                                var scheme = Format.scheme_from_url(link)

                                if (scheme === "http" || scheme === "https")
                                {
                                    webLinkMenu.link = link
                                    webLinkMenu.popup()
                                }
                                else if (scheme === "kulloInternal")
                                {
                                    kulloAddressLinkMenu.address = link.substring(scheme.length+1)
                                    kulloAddressLinkMenu.popup()
                                }
                                else
                                {
                                    console.warn("Unknown scheme: " + scheme)
                                }
                            }
                        }
                    }
                }

                MessageDelegateAttachmentsBox {
                    id: attachmentsBox
                    conversationId: root.conversationId
                    messageId: root.messageId
                    attachmentsReady: root.attachmentsReady
                    attachments: root.attachments

                    anchors {
                        top: messageTextBox.bottom
                        left: parent.left
                        right: parent.right
                    }
                    height: (!root.attachments || root.attachments.count() === 0)
                            ? 0
                            : implicitHeight

                    opacity: root.attachmentsReady ? 1 : 0.3

                    Component.onCompleted: {
                        saveAttachmentRequested.connect(root.saveAttachmentRequested)
                        saveAttachmentsRequested.connect(root.saveAttachmentsRequested)
                    }
                }
            }

            //Rectangle { color: "#8800ff00"
            Item {
                id: footerContainer

                anchors {
                    top: body.bottom
                    left: parent.left
                    right: parent.right
                }

                height: footerBorderTop.height
                        + Math.max(footerText.height, deleteBottom.height)
                        + 2*deleteBottom.anchors.topMargin

                Rectangle {
                    id: footerBorderTop
                    anchors {
                        top: parent.top
                        left: parent.left
                        leftMargin: paddingLeft
                        right: parent.right
                        rightMargin: paddingRight
                    }
                    height: 1
                    color: "#ddd"
                }

                Item {
                    id: toggleFooterButton
                    visible: _hasFooter
                    anchors {
                        left: parent.left
                        leftMargin: 15
                        top: footerBorderTop.bottom
                        topMargin: 5
                    }

                    width: _showFooter
                           ? img.width
                           : img.width + txt.anchors.leftMargin + txt.width
                    height: txt.contentHeight

                    NativeImage {
                        id: img
                        width: 14
                        height: 14
                        anchors {
                            left: parent.left
                            top: parent.top
                            topMargin: Math.floor((parent.height-height) / 2)
                        }
                        source: _showFooter
                                ? "/resources/scalable/chevron-circle-bottom_b.svg"
                                : "/resources/scalable/chevron-circle-right_b.svg"
                    }

                    NativeText {
                        id: txt
                        width: contentWidth
                        wrapMode: Text.NoWrap
                        anchors {
                            left: img.right
                            leftMargin: 6
                            top: parent.top
                        }
                        text: qsTr("Footer")
                        visible: !_showFooter
                        color: Style.messageFooter
                    }

                    MouseArea {
                        anchors.fill: parent

                        onClicked: {
                            _showFooter = !_showFooter
                        }
                    }
                }

                NativeSelectableText {
                    id: footerText
                    text: footer

                    color: Style.messageFooter
                    height: _showFooter ? implicitHeight : 0
                    clip: true

                    anchors {
                        top: footerBorderTop.bottom
                        topMargin: deleteBottom.anchors.topMargin
                        left: parent.left
                        leftMargin: 35
                        right: deleteBottom.left
                    }

                    CutCopyPasteMenu {
                        hasCopy: true
                        hasPaste: false
                        hasCut: false
                        hasSelectAll: true

                        onCopy: footerText.copy()
                        onSelectAll: footerText.selectAll()
                    }
                }

                IconButton {
                    id: deleteBottom
                    anchors {
                        right: parent.right
                        rightMargin: 5
                        top: footerBorderTop.bottom
                        topMargin: 5
                    }
                    tooltip: qsTr("Delete message")
                    source: "/resources/scalable/delete_b.svg"
                    visible: !!messagesList

                    onClicked: {
                        var deletionStarted = messagesList.triggerDelayedDeletion(messageId, deleteAnimationDuration + 50)
                        if (deletionStarted) {
                            root.state = "deleting"
                        }
                    }
                }
            }
        }
    }
}
