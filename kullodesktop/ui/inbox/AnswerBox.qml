/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
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
    /* public */
    property Conversation conversation

    /* private */
    readonly property bool _enabled: conversation && conversation.draft.state === "editing"
    readonly property bool _empty: !conversation || conversation.draft.empty
    readonly property bool _userSettingsReadyForSending: Inbox.userSettings.name.trim() !== ""

    id: root
    anchors.fill: parent

    ErrorDialog {
        id: addAttachmentErrorDialog
    }

    onConversationChanged: {
        // initial text backend->UI
        var newText = conversation ? conversation.draft.text : ""
        messageText.cursorPosition = 0
        messageText.text = newText
        messageText.cursorPosition = newText.length
    }

    Connections {
        target: root.conversation
                ? root.conversation.draft
                : null

        // text updates backend->UI
        onTextChanged: {
            var newText = root.conversation.draft.text

            // avoid warning when new text is shorter than old cursor position
            if (messageText.cursorPosition > newText.length) {
                messageText.cursorPosition = 0
            }

            messageText.text = newText
        }
    }

    AttachmentsAdder {
        id: attachmentsAdder
        target: root.conversation ? root.conversation.draft : null
        onProgressed: {
            workingIndicator.indeterminate = false
            workingIndicator.value = percentages
        }
        onAddingAttachmentsStarted: {
            workingIndicator.indeterminate = true
            workingIndicator.visible = true
        }
        onAddingAttachmentsError: {
            addAttachmentErrorDialog.title = qsTr("Adding attachment failed")
            switch (error) {
            case LocalError.FileTooBig:
                addAttachmentErrorDialog.text = qsTr("The file %1 is too big to be added to the draft").arg(filename)
                break;
            case LocalError.Filesystem:
                addAttachmentErrorDialog.text = qsTr("Error reading the file %1").arg(filename)
                break;
            default:
                addAttachmentErrorDialog.text = qsTr("Unknown error when adding the file %1").arg(filename)
            }
            addAttachmentErrorDialog.open()

            workingIndicator.visible = false
        }
        onAddingAttachmentsFinished: {
            workingIndicator.visible = false
        }
    }

    function addQuoteToAnswer(quoteText) {
        var draftText = messageText.text.toString()
        var cursorPos = messageText.cursorPosition
        var len = messageText.text.toString().length
        var pre = draftText.substring(0, cursorPos)
        var post = draftText.substring(cursorPos)
        messageText.text = pre + '"' + quoteText + '"' + post
    }

    ErrorDialog {
        id: setUserSettingsPrompt
        title: qsTr("Profile not ready")
        text: qsTr("Profile not ready for sending messages.") + "\n"
              + qsTr("Set now?")
        standardButtons: StandardButton.Yes | StandardButton.Cancel
        onYes: {
            profileOverlay.fadeIn()
        }
        onRejected: {
            // just do the default action: close dialog
        }
    }

    function tryToSend()
    {
        if (!_userSettingsReadyForSending)
        {
            setUserSettingsPrompt.open()
        }
        else if (_empty)
        {
            console.debug("tryToSend(): Draft is empty. Not sent.")
        }
        else
        {
            console.debug("Send message ...")
            conversation.draft.prepareToSend()
            heartbeat.syncAsap()
        }
    }

    SystemPalette {
        id: palette
        colorGroup: root._enabled ? SystemPalette.Active : SystemPalette.Disabled
    }

    SaveUnit {
        id: saveUnit
        onSave: root.conversation.draft.save()
    }

    Keys.onPressed: {
        //console.log("FocusScope:" + SC.eventToString(event));

        if (SC.isPlainKey(Qt.Key_Escape, event))
        {
            event.accepted = true
            inboxScreen.closeAnswer()
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
                left: parent.left
                right: parent.right
                bottom: footer.top
                bottomMargin: 5 * Hdpi.FontScalingFactor
            }
            color: "white"
            radius: messageText.anchors.margins * 0.666

            property int _padding: 8 * Hdpi.FontScalingFactor

            AnswerBoxHeader {
                id: header
                anchors {
                    top: parent.top
                    left: parent.left
                    right: parent.right
                    topMargin: body._padding
                    leftMargin: body._padding
                    rightMargin: body._padding
                }
                conversation: root.conversation
            }

            Item {
            //Rectangle { color: "green"
                id: inputContainer

                anchors {
                    top: header.bottom
                    left: parent.left;
                    right: parent.right
                    bottom: attachmentsBox.top
                    margins: body._padding
                }

                EmojiPopup {
                    id: emojiPopup
                    z: 2

                    onSelected: {
                        messageText.insert(messageText.cursorPosition, emoji)
                        messageText.forceActiveFocus()
                        close()
                    }

                    onCanceled: {
                        messageText.forceActiveFocus()
                        close()
                    }
                }

                TextArea {
                    id: messageText
                    anchors.fill: parent
                    focus: true
                    enabled: root._enabled
                    textColor: palette.text

                    font.family: InnerApplication.deviceSettings.messagesFontPreview.family
                    font.italic: InnerApplication.deviceSettings.messagesFontPreview.italic
                    font.weight: InnerApplication.deviceSettings.messagesFontPreview.weight
                    font.pointSize: Style.fontSize.answerTextInput

                    wrapMode: TextEdit.Wrap

                    // Do not use property binding for `text` here because
                    // the two-way signaling is too complex to work reliably

                    Keys.onPressed: {
                        if (SC.isCtrlAndKey(Qt.Key_Period, event)) {
                            event.accepted = true

                            var pos = messageText.cursorRectangle
                            var posInContainer = mapToItem(inputContainer, pos.x, pos.y, pos.width, pos.height)
                            //console.log(pos)
                            var xCenter = posInContainer.x + (posInContainer.width/2.0)
                            var yBottom = posInContainer.y + posInContainer.height
                            var yTop = posInContainer.y
                            if (yBottom + emojiPopup.height < inputContainer.height - 10) {
                                emojiPopup.placeBelow(xCenter, yBottom)
                            }
                            else {
                                emojiPopup.placeAbove(xCenter, yTop)
                            }
                            emojiPopup.open()
                        }
                    }

                    // text change UI->backend
                    onTextChanged: {
                        if (root.conversation) {
                            root.conversation.draft.text = text
                        }
                        saveUnit.count()
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
            }

            Item {
                id: attachmentsBox

                anchors {
                    bottom: parent.bottom
                    left: parent.left
                    right: parent.right
                    leftMargin: body._padding
                    rightMargin: body._padding
                    bottomMargin: body._padding
                }
                height: Math.max(addAttachmentsBox.height, attachmentsList.height)

                Menu {
                    id: contextMenu
                    property int attachmentIndex: -1

                    MenuItem {
                        text: qsTr("Open")
                        onTriggered: {
                            root.conversation.draft.attachments.get(contextMenu.attachmentIndex).openAsync()
                        }
                    }

                    MenuItem {
                        text: qsTr("Remove attachment")
                        onTriggered: {
                            root.conversation.draft.removeAttachment(contextMenu.attachmentIndex)
                        }
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

                    CircularProgressBar {
                        id: workingIndicator
                        property int _padding: 6 * Hdpi.FontScalingFactor
                        anchors {
                            top: parent.top
                            right: parent.right
                            margins: _padding
                        }
                        width: addAttachmentsButton.width - 2*_padding
                        height: addAttachmentsButton.height - 2*_padding
                        color: Style.gray
                        visible: false
                        minimumValue: 0
                        maximumValue: 100
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
                        leftMargin: 15 * Hdpi.FontScalingFactor
                        rightMargin: 0
                    }
                    property int cols: Math.floor(width/cellWidth)
                    height: cellHeight*Math.ceil(attachmentsList.count/cols)
                    cellWidth: 145 * Hdpi.FontScalingFactor
                    cellHeight: 85 * Hdpi.FontScalingFactor

                    clip: true
                    interactive: false
                    model: conversation ? root.conversation.draft.attachments : null

                    delegate: DraftAttachmentDelegate {
                        width: attachmentsList.cellWidth
                        height: attachmentsList.cellHeight
                        conversationId: root.conversation.id

                        function removeAttachment() {
                            root.conversation.draft.removeAttachment(attachmentIndex_)
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
                                var attachment = root.conversation.draft.attachments.get(attachmentIndex_)
                                if (!attachment.openAsync()) {
                                    inboxScreen.showOpenFileError(attachment.filename)
                                }
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
                enabled: root._enabled && !root._empty
                onClicked: root.tryToSend()
                style: KulloButtonStyle {
                    source: "/resources/scalable/send_w.svg"
                }
            }
        }
    }
}
