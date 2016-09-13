/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Controls 1.3
import Kullo 1.0

import "../"
import "../buttons"
import "../misc"
import "../native"
import "../js/shortcut.js" as SC

Item {
    /* public */
    property var messagesList
    property int paddingTop: 10
    property int paddingRight: 10
    property int paddingBottom: 10
    property int paddingLeft: 10
    property int conversationId: conversationId_
    property int messageId: id_
    property bool attachmentsReady: attachmentsReady_
    property string highlightColor: "#bbbbbb"

    /* private */
    property bool _selected: ListView.isCurrentItem
    property bool _showFooter: false
    property bool _hasFooter: footer_.trim() !== ""
    property real _maxTextWidth: 600*Hdpi.FontScalingFactor

    signal avatarClicked();
    signal avatarDoubleClicked();

    id: root

    anchors {
        left: parent.left
        right: parent.right
    }
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
        NumberAnimation { properties: "opacity,height"; duration: deleteTimer.interval }
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
        messagesList.currentIndex = index
    }

    // Keep border a separate component because "The border is rendered within the
    // rectangle's boundaries." so offsetting every child would be painful otherwise.
    Rectangle {
        id: messageBorder
        color: "transparent"
        border {
            color: _selected ? highlightColor : "transparent"
            width: 1
        }

        y: paddingTop
        anchors {
            left: parent.left
            right: parent.right
            leftMargin: paddingLeft
            rightMargin: paddingRight
        }
        height: messageBackground.height + 2

        Rectangle {
            id: messageBackground
            color: Style.messageBackground

            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
                topMargin: 1
                leftMargin: 1
                rightMargin: 1
            }
            height: header.height + body.height + footer.height

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    messagesList.currentIndex = index
                    messagesList.forceActiveFocus()
                }
            }

            MessageHeader {
                id: header
                date: dateReceived_
                showMessageState: incoming_
                showMessageDeliveryStatus: !incoming_
                messageDeliveryStatus: deliveryStatus_
                read: read_
                done: done_
                name: senderName_
                organization: senderOrganization_
                address: senderAddress_
                avatarSource: "image://messageavatars/" + Utils.urlencode(Inbox.userSettings.address) + "/" + conversationId_ + "/" + id_

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
                    text: "<div style='white-space: pre-wrap;'>" + textAsHtml_ + "</div>"
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
                    onLinkActivated: Qt.openUrlExternally(link)

                    function quoteSelectedText() {
                        var quoteText = messageTextBox.selectedText.trim()
                        inbox.openAnswer()
                        rightColumn.addQuoteToAnswer(quoteText)
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
                        id: linkMenu
                        property string link: ""

                        MenuItem {
                            text: qsTr("Copy link address")
                            onTriggered: Utils.setClipboardText(linkMenu.link)
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
                                if (link !== "")
                                {
                                    linkMenu.link = link
                                    linkMenu.popup()
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

                    anchors {
                        top: messageTextBox.bottom
                        left: parent.left
                        right: parent.right
                    }
                    height: (attachments_.count() === 0)
                            ? 0
                            : implicitHeight

                    opacity: root.attachmentsReady ? 1 : 0.3
                }
            }

            //Rectangle { color: "#8800ff00"
            Item {
                id: footer

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
                    text: footer_

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

                    Timer {
                        id: deleteTimer
                        interval: 350
                        onTriggered: {
                            messagesList.model.deleteMessage(id_)
                        }
                    }

                    onClicked: {
                        root.state = "deleting"
                        deleteTimer.start()
                    }
                }
            }
        }
    }
}
