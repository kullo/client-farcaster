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
    id: _root

    property int paddingTop: 10
    property int paddingRight: 10
    property int paddingBottom: 10
    property int paddingLeft: 10

    anchors.left: parent.left
    anchors.right: parent.right
    height: paddingTop + messageBorder.height + paddingBottom + intro.height
    clip: true

    property var conversationParticipantsAddresses: [] // stringlist
    property int conversationId: conversationId_
    property int messageId: id_
    property bool attachmentsReady: attachmentsReady_

    property bool selected: index == messagesList.currentIndex
    property string highlightColor: "#bbbbbb"

    property bool showFooter: false
    property bool hasFooter: footer_.trim() !== ""

    signal avatarClicked();
    signal avatarDoubleClicked();

    states: State {
        name: "deleting"
        PropertyChanges {
            target: _root
            opacity: 0
            height: 0
        }
    }

    transitions: Transition {
        NumberAnimation { properties: "opacity,height"; duration: deleteTimer.interval }
    }

    onSelectedChanged: {
        if (selected) {
            if (!read_) markAsRad()
        }
    }

    Component.onCompleted: checkForLastElement()

    function checkForLastElement()
    {
        if (index == -1)
        {
            intro.visible = false
        }
        else
        {
            if (index == messagesList.count-1)
            {
                intro.visible = true
            }
            else
            {
                intro.visible = false
            }
        }
    }

    function markAsRad() {
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

    Rectangle {
        id: messageBorder
        color: "transparent"
        border.color: selected ? highlightColor : "transparent"
        border.width: 1

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
            height: _header.height + _body.height + _footer.height

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    messagesList.currentIndex = index
                    messagesList.forceActiveFocus()
                }
            }

            MessageHeader {
                id: _header
                dateSent: dateSent_
                showMessageState: incoming_
                showMessageDeliveryStatus: !incoming_
                messageDeliveryStatus: deliveryStatus_
                read: read_
                done: done_
                name: senderName_
                organization: senderOrganization_
                address: senderAddress_
                avatarSource: "image://messageavatars/" + Utils.urlencode(Client.userSettings.address) + "/" + conversationId_ + "/" + id_

                onAvatarClicked: _root.avatarClicked()
                onAvatarDoubleClicked: _root.avatarDoubleClicked()
            }

            Item {
                id: _body
                anchors {
                    top: _header.bottom
                    left: parent.left
                    right: parent.right
                }
                height: _messageTextBox.anchors.topMargin
                      + _messageTextBox.height
                      + _messageTextBox.anchors.bottomMargin
                      + _attachmentsBox.height

                NativeSelectableText {
                    id: _messageTextBox
                    text: "<div style='white-space: pre-wrap;'>" + textAsHtml_ + "</div>"
                    anchors {
                        top: parent.top
                        left: parent.left
                        topMargin: 20
                        leftMargin: 15
                        bottomMargin: 20
                    }
                    width: (_body.width-2*anchors.leftMargin) < 600 ? (_body.width-2*anchors.leftMargin) : 600
                    font.family: Devicesettings.messagesFontPreview.family
                    font.italic: Devicesettings.messagesFontPreview.italic
                    font.weight: Devicesettings.messagesFontPreview.weight
                    font.pointSize: Style.fontSize.message
                    color: Style.messageText
                    textFormat: TextEdit.RichText
                    onLinkActivated: Qt.openUrlExternally(link)

                    function quoteSelectedText() {
                        var quoteText = _messageTextBox.selectedText.trim()
                        inbox.openAnswer()
                        rightColumn.addQuoteToAnswer(quoteText)
                    }

                    Menu {
                        id: _contextMenu

                        MenuItem {
                            text: qsTr("Copy")
                                  + " (%1)".arg(SC.nameOfCtrlAndKey("C", Os.osx))
                            onTriggered: _messageTextBox.copy()
                        }
                        MenuItem {
                            text: qsTr("Quote")
                                  + " (%1)".arg(SC.nameOfCtrlAndKey("B", Os.osx))
                            onTriggered: _messageTextBox.quoteSelectedText()
                        }
                    }

                    Menu {
                        id: _linkMenu
                        property string link: ""

                        MenuItem {
                            text: qsTr("Copy link address")
                            onTriggered: Utils.setClipboardText(_linkMenu.link)
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
                            if (_messageTextBox.selectionStart != _messageTextBox.selectionEnd)
                            {
                                _contextMenu.popup()
                            }
                            else
                            {
                                var link = _messageTextBox.linkAt(mouseX, mouseY)
                                if (link !== "")
                                {
                                    _linkMenu.link = link
                                    _linkMenu.popup()
                                }
                            }
                        }
                    }
                }

                MessageDelegateAttachmentsBox {
                    id: _attachmentsBox

                    anchors {
                        top: _messageTextBox.bottom
                        left: parent.left
                        right: parent.right
                    }
                    height: (attachments_.count() === 0)
                            ? 0
                            : implicitHeight

                    opacity: attachmentsReady_ ? 1 : 0.3
                }
            }

            Rectangle {
                id: _footer
                color: "transparent"

                anchors {
                    top: _body.bottom
                    left: parent.left
                    right: parent.right
                }

                height: _footerBorderTop.height
                        + Math.max(_footerText.height, deleteBottom.height)
                        + 2*deleteBottom.anchors.topMargin

                Rectangle {
                    id: _footerBorderTop
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
                    visible: hasFooter
                    anchors {
                        left: parent.left
                        leftMargin: 15
                        top: _footerBorderTop.bottom
                        topMargin: 5
                    }

                    width: showFooter
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
                        source: showFooter ?
                                    "/resources/scalable/chevron-circle-bottom_b.svg"
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
                        visible: !showFooter
                        color: Style.messageFooter
                    }

                    MouseArea {
                        anchors.fill: parent

                        onClicked: {
                            showFooter = !showFooter
                        }
                    }
                }

                NativeSelectableText {
                    id: _footerText
                    text: footer_

                    color: Style.messageFooter
                    height: showFooter ? implicitHeight : 0
                    clip: true

                    anchors {
                        top: _footerBorderTop.bottom
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

                        onCopy: _footerText.copy()
                        onSelectAll: _footerText.selectAll()
                    }
                }

                IconButton {
                    id: deleteBottom
                    anchors {
                        right: parent.right
                        rightMargin: 5
                        top: _footerBorderTop.bottom
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
                        _root.state = "deleting"
                        deleteTimer.start()
                    }
                }
            }
        }
    }

    ConversationIntro {
        id: intro
        anchors {
            left: parent.left
            leftMargin: paddingLeft
            right: parent.right
            rightMargin: paddingRight
        }
        y: messageBackground.y + messageBackground.height + paddingBottom

        Connections {
            target: messagesList
            onCountChanged: checkForLastElement()
        }
    }
}
