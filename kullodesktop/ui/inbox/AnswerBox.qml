/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3
import QtQuick.Dialogs 1.2
import Kullo 1.0

import ".."
import "../buttons"
import "../dialogs"
import "../misc"
import "../native"
import "../js/shortcut.js" as SC

FocusScope {
    anchors.fill: parent

    id: answer

    property int conversationId: -1

    property bool enabled: conversationId != -1 && Client.conversations.get(conversationId).draft.state === "editing"
    property bool empty: conversationId == -1
                         || Client.conversations.get(conversationId).draft.empty

    onConversationIdChanged: {
        messageText.reloadTextFromDraftModel()
    }

    AttachmentsAdder {
        id: attachmentsAdder
        target: conversationId != -1
            ? Client.conversations.get(conversationId).draft
            : null
        onAddingAttachmentsStarted: workingIndicator.visible = true
        onAddingAttachmentsFinished: workingIndicator.visible = false
    }

    function addQuoteToAnswer(quoteText) {
        var drafText = messageText.text.toString()
        var cursorPos = messageText.cursorPosition
        var len = messageText.text.toString().length
        var pre = drafText.substring(0, cursorPos)
        var post = drafText.substring(cursorPos)
        messageText.text = pre + '"' + quoteText + '"' + post
    }

    function tryToSend()
    {
        if (!empty)
        {
            console.debug("Send message ...")
            Client.conversations.get(conversationId).draft.prepareToSend()
            heartbeat.syncAsap()
        }
        else
        {
            console.debug("tryToSend(): Draft is empty. Not sent.")
        }
    }

    function refreshAvatar() {
        messageHeader.avatarSource = "image://senderavatars/" + Client.userSettings.address + "?" + Date.now()
    }

    SystemPalette {
        id: palette
        colorGroup: answer.enabled ? SystemPalette.Active : SystemPalette.Disabled
    }

    SaveUnit {
        id: saveUnit
        onSave: Client.conversations.get(conversationId).draft.save()
    }

    Keys.onPressed: {
        //console.log("FocusScope:" + SC.eventToString(event));

        if (SC.isPlainKey(Qt.Key_Escape, event))
        {
            event.accepted = true
            inbox.closeAnswer()
        }

        if (SC.isCtrlAndKey(Qt.Key_Return, event))
        {
            event.accepted = true
            tryToSend()
        }
    }

    DropArea {
        anchors.fill: parent

        onDropped: {
            if (drop.hasUrls && drop.action == Qt.CopyAction)
            {
                console.debug("Dropped URLs " + drop.urls)
                attachmentsAdder.addAttachments(drop.urls)
                drop.acceptProposedAction()
            }
            else {
                drop.accepted = false
            }
        }

        Rectangle {
            id: body
            anchors {
                top: parent.top
                topMargin: 5
                left: parent.left
                right: parent.right
                bottom: footer.top
                bottomMargin: 5
            }
            color: "white"

            MessageHeader {
                id: messageHeader
                anchors {
                    top: parent.top
                    left: parent.left
                    right: parent.right
                }
                showMessageState: false

                name: Client.userSettings.name
                organization: Client.userSettings.organization
                address: Client.userSettings.address
                avatarSource: "image://senderavatars/" + Client.userSettings.address
            }

            TextArea {
                id: messageText
                anchors {
                    top: messageHeader.bottom
                    left: parent.left;
                    right: parent.right
                    bottom: attachmentsBox.top
                    margins: 5
                }
                focus: true
                enabled: answer.enabled
                textColor: palette.text
                font.pointSize: Style.fontSize.answerTextInput
                font.family: FontList.SerifFamily
                wrapMode: TextEdit.Wrap

                function reloadTextFromDraftModel()
                {
                    var modelText = (conversationId != -1)
                            ? Client.conversations.get(conversationId).draft.text
                            : ""
                    if (text !== modelText)
                    {
                        text = modelText
                    }
                }

                onTextChanged: {
                    console.debug("TextArea.textChanged(\"" + text + "\")")
                    var conv = Client.conversations.get(conversationId)
                    if (!conv) return

                    if (!conv.draft) console.warn("draft is null in AnswerBox");

                    conv.draft.text = text
                    saveUnit.count()
                }

                Connections {
                    target: conversationId != -1
                        ? Client.conversations.get(conversationId).draft
                        : null
                    onTextCleared: messageText.text = ""
                    onTextPrepared: messageText.reloadTextFromDraftModel()
                }

                CutCopyPasteMenu {
                    hasCut: true
                    hasSelectAll: true
                    onCut: messageText.cut()
                    onCopy: messageText.copy()
                    onPaste: messageText.paste()
                    onSelectAll: messageText.selectAll()
                }
            }

            Item {
                id: attachmentsBox

                anchors {
                    bottom: parent.bottom
                    left: parent.left
                    right: parent.right
                    leftMargin: 5
                    rightMargin: 5
                    bottomMargin: 5
                }
                height: Math.max(addAttachmentsBox.height, attachmentsList.height)

                Menu {
                    id: contextMenu
                    property int attachmentIndex: -1

                    MenuItem {
                        text: qsTr('Remove attachment')
                        onTriggered:
                            Client.conversations.get(conversationId).draft.removeAttachment(contextMenu.attachmentIndex)
                    }
                }

                Rectangle {
                    id: addAttachmentsBox
                    anchors {
                        left: parent.left
                        top: parent.top
                    }
                    width: addAttachmentsButton.width
                    height: addAttachmentsButton.height

                    IconButton {
                        id: addAttachmentsButton
                        anchors {
                            top: parent.top
                            right: parent.right
                        }
                        visible: !workingIndicator.visible
                        tooltip: qsTr("Add attachments")
                        source: "/resources/scalable/attach_b.svg"
                        onClicked: fileDialog.openDialog()
                    }

                    AnimatedImage {
                        id: workingIndicator
                        anchors {
                            top: parent.top
                            right: parent.right
                        }
                        width: addAttachmentsButton.width
                        height: addAttachmentsButton.height
                        source: "/resources/working-indicator.gif"
                        fillMode: Image.Pad
                        verticalAlignment: Image.AlignVCenter
                        horizontalAlignment: Image.AlignHCenter
                        visible: false
                    }

                    AdvancedFileOpenDialog {
                        id: fileDialog
                        title: qsTr("Add attachments")
                        selectMultiple: true
                        onAccepted: {
                            console.info("Selected Files: " + fileUrls)
                            attachmentsAdder.addAttachments(fileUrls);
                        }
                    }
                }

                GridView {
                    id: attachmentsList
                    anchors {
                        left: addAttachmentsBox.right
                        right: parent.right
                        bottom: parent.bottom
                        leftMargin: 15
                        rightMargin: 0
                    }
                    property int cols: Math.floor(width/cellWidth)
                    height: cellHeight*Math.ceil(attachmentsList.count/cols)
                    cellWidth: 145
                    cellHeight: 85

                    clip: true
                    interactive: false
                    model: conversationId != -1 ? Client.conversations.get(conversationId).draft.attachments : null

                    delegate: DraftAttachmentDelegate {
                        width: attachmentsList.cellWidth
                        height: attachmentsList.cellHeight

                        function removeAttachment() {
                            Client.conversations.get(conversationId).draft.removeAttachment(attachmentIndex_)
                        }

                        Keys.onPressed: {
                            if (Os.osx && event.key === Qt.Key_Backspace) {
                                removeAttachment()
                            }
                        }
                        Keys.onDeletePressed: removeAttachment()

                        MouseArea {
                            anchors.fill: parent
                            acceptedButtons: Qt.LeftButton | Qt.RightButton
                            propagateComposedEvents: false
                            onClicked: {
                                attachmentsList.forceActiveFocus()
                                attachmentsList.currentIndex = index

                                if (mouse.button === Qt.RightButton) {
                                    contextMenu.attachmentIndex = attachmentIndex_
                                    contextMenu.popup()
                                }
                            }
                            onDoubleClicked: {
                                Client.conversations.get(conversationId).draft.attachments.get(attachmentIndex_).open()
                            }
                        }
                    }

                    onCurrentIndexChanged: forceActiveFocus()
                }
            }
        }

        Item {
            id: footer
            height: sendButton.height

            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }

            NativeButton {
                id: sendButton
                anchors {
                    right: parent.right
                    bottom: parent.bottom
                }
                text: qsTr("Send")
                tooltip: qsTr("Send message")
                         //: Name of the big key below backspace
                         + " (%1)".arg(SC.nameOfCtrlAndKey(qsTr("Return"), Os.osx))
                enabled: answer.enabled && !answer.empty
                onClicked: answer.tryToSend()
                style: KulloButtonStyle {
                    source: "/resources/scalable/send_w.svg"
                }
            }
        }
    }
}
