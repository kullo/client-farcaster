/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import Kullo 1.0

import ".."
import "../buttons"
import "../dialogs"
import "../native"
import "../misc"
import "../windows"
import "../js/dynamic.js" as Dynamic
import "../js/shortcut.js" as SC

BackgroundCover {
    /* public */
    state: "hidden"
    property alias contentWidth: frame.width
    property alias contentHeight: frame.height
    property alias searchText: searchInput.text
    property bool global: true
    property int conversationId
    function updateResults() {
        // Hide empty state assuming there will be results. Otherwise empty state
        // view shows an updated query text for a search that is not finished
        emptyState.visible = false

        searchModel.query = searchInput.text
        searchModel.conversationId = global ? -1 : conversationId
        searchModel.direction = direction
        searchModel.updateResults()
    }
    property string direction: "both"

    /* private */
    id: root
    property bool _doneLoading: false
    property var _messageDelegateComponent: null
    property int _HEADER_CONTENT_SPACING: 25 * Hdpi.FontScalingFactor

    onDirectionChanged: {
        if (direction == "both") messageDirectionBoth.checked = true
        else if (direction == "incoming") messageDirectionIncoming.checked = true
        else if (direction == "outgoing") messageDirectionOutgoing.checked = true
    }

    onGlobalChanged: {
        if (global) {
            conversationFilterGlobal.checked = true
        } else {
            conversationFilterConversation.checked = true
        }
    }


    onConversationIdChanged: {
        var modelIndex = conversationSelection.model.find(conversationId)
        conversationSelection.currentIndex = modelIndex
    }

    onStateChanged: {
        if (_doneLoading)
        {
            // this code must not be called when state is set initially

            if (state == "visible") {
            }

            if (state == "hidden") {
            }
        }
    }

    onFadedIn: {
        searchInput.forceActiveFocus()
    }

    Component.onCompleted: {
        _doneLoading = true
    }

    function openMessage(conversationId, messageId) {
        for (var i = singleMessageContainer.children.length; i > 0 ; --i) {
            singleMessageContainer.children[i-1].destroy()
        }

        if (conversationId === -1 || messageId === -1) return

        if (!_messageDelegateComponent) {
            _messageDelegateComponent = Dynamic.createComponentOrThrow("/inbox/MessageDelegate.qml")
        }

        var msg = Inbox.allConversations.get(conversationId).messages.get(messageId)
        var properties = {
            'conversationId': conversationId,
            'messageId': messageId,
            'textAsHtml': msg.textAsHtml,
            'footer': msg.footer,
            'senderAddress': msg.sender.address,
            'senderName': msg.sender.name,
            'senderOrganization': msg.sender.organization,
            'read': msg.read,
            'done': msg.done,
            'deliveryStatus': "none",
            'incoming': msg.sender.address !== Inbox.userSettings.address,
            'dateReceived': msg.dateReceived,
            'attachments': msg.attachments,
            'attachmentsReady': msg.attachmentsReady,

            'paddingTop': 0,
            'paddingRight': 0,
            'paddingBottom': 0,
            'paddingLeft': 0,
            'borderWidth': 0,
            'width': Qt.binding(function() {
                return content.width
                        - 400 * Hdpi.FontScalingFactor // search results
                        - 20 * Hdpi.FontScalingFactor // spacing
                        - 20 * Hdpi.FontScalingFactor // scrollbar width
            }),
        }

        // Create synchronously to avoid flicker effect: deletion of old
        // and layouting new messages happens in one frame
        var msg = _messageDelegateComponent.createObject(singleMessageContainer, properties)
        msg.saveAttachmentRequested.connect(inboxScreen.openSaveAttachmentDialog)
        msg.saveAttachmentsRequested.connect(inboxScreen.openSaveAttachmentsDialog)
        msg.quoteRequested.connect(function(quoteText) {
            root.quote(conversationId, messageId, quoteText)
        })
    }

    function showMessageInInbox(conversationId, messageId) {
        inboxScreen.openConversation(conversationId)
        middleColumn.selectMessage(messageId)
        fadeOut()
    }

    function quote(conversationId, messageId, quoteText) {
        inboxScreen.openConversation(conversationId)
        middleColumn.selectMessage(messageId)
        inboxScreen.openAnswer()
        rightColumn.addQuoteToAnswer(quoteText)
        fadeOut()
    }

    Rectangle {
        id: frame
        anchors {
            horizontalCenter: parent.horizontalCenter
            verticalCenter: parent.verticalCenter
        }
        radius: 10 * Hdpi.FontScalingFactor

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.AllButtons
            onClicked: {} // do nothing. Prevents overlay from closing
        }

        IconButton {
            anchors {
                top: parent.top
                right: parent.right
                topMargin: 8 * Hdpi.FontScalingFactor
                rightMargin: 10 * Hdpi.FontScalingFactor
            }
            tooltip: qsTr("Close (Esc)")
            source: "/resources/scalable/close2_b.svg"
            onClicked: fadeOut()
        }

        Item {
            id: content
            anchors {
                fill: parent
                margins: 15 * Hdpi.FontScalingFactor
            }

            Flow {
                id: header
                anchors {
                    top: parent.top
                    left: parent.left
                    right: parent.right
                }
                property int _SEARCH_INPUT_DEFAULT_WIDTH: 250 * Hdpi.FontScalingFactor
                property int _HORIZONTAL_SPACING: 40 * Hdpi.FontScalingFactor
                property int _VERTICAL_SPACING: 5 * Hdpi.FontScalingFactor
                property bool showOneLine: header.width >= (
                                               + _SEARCH_INPUT_DEFAULT_WIDTH
                                               + _HORIZONTAL_SPACING
                                               + directionFilter.implicitWidth
                                               + _HORIZONTAL_SPACING
                                               + conversationFilterBox.implicitWidth
                                               + 50 * Hdpi.FontScalingFactor // right space for close button and some fresh air
                                               )
                spacing: _VERTICAL_SPACING

                TextField {
                    id: searchInput
                    width: header.showOneLine
                           ? header._SEARCH_INPUT_DEFAULT_WIDTH
                           : parent.width - 100 * Hdpi.FontScalingFactor
                    placeholderText: root.global
                                     ? qsTr("Search Kullo")
                                     : qsTr("Search conversation")
                    font.pointSize: Style.fontSize.bigTextInput
                    focus: true
                    KeyNavigation.tab: searchResults

                    onTextChanged: updateResults()
                }

                Item {
                    height: 1 * Hdpi.FontScalingFactor
                    width: header.showOneLine
                           ? (header._HORIZONTAL_SPACING - 2*header._VERTICAL_SPACING)
                           : 0
                }

                Row {
                    id: directionFilter
                    spacing: 8 * Hdpi.FontScalingFactor

                    ExclusiveGroup {
                        id: messageDirection
                        onCurrentChanged: {
                            if (current == messageDirectionBoth) root.direction = "both"
                            else if (current == messageDirectionIncoming) root.direction = "incoming"
                            else if (current == messageDirectionOutgoing) root.direction = "outgoing"
                            updateResults()
                        }
                    }
                    RadioButton {
                        id: messageDirectionBoth
                        exclusiveGroup: messageDirection
                        text: qsTr("all")
                        checked: true
                        height: searchInput.height
                    }
                    RadioButton {
                        id: messageDirectionIncoming
                        exclusiveGroup: messageDirection
                        text: qsTr("received")
                        height: searchInput.height
                    }
                    RadioButton {
                        id: messageDirectionOutgoing
                        exclusiveGroup: messageDirection
                        text: qsTr("sent")
                        height: searchInput.height
                    }
                }

                Item {
                    height: 1 * Hdpi.FontScalingFactor
                    width: (header._HORIZONTAL_SPACING - 2*header._VERTICAL_SPACING)
                }

                Row {
                    id: conversationFilterBox
                    spacing: 8 * Hdpi.FontScalingFactor

                    ExclusiveGroup {
                        id: conversationFilter
                        onCurrentChanged: {
                            if (current == conversationFilterGlobal) {
                                root.global = true
                            } else if (current == conversationFilterConversation) {
                                root.global = false
                            }
                            updateResults()
                        }
                    }

                    RadioButton {
                        id: conversationFilterGlobal
                        text: qsTr("everywhere")
                        exclusiveGroup: conversationFilter
                        height: searchInput.height
                        checked: true
                    }

                    Row {
                        spacing: 4 * Hdpi.FontScalingFactor
                        RadioButton {
                            id: conversationFilterConversation
                            text: qsTr("in conversation")
                            exclusiveGroup: conversationFilter
                            height: searchInput.height
                        }

                        ComboBox {
                            id: conversationSelection
                            model: Inbox.allConversations
                            textRole: "titleShort_"
                            width: 160 * Hdpi.FontScalingFactor

                            onCurrentIndexChanged: {
                                var modelIndex = currentIndex
                                root.conversationId = model.at(modelIndex)["id_"]
                                root.updateResults()
                            }
                        }
                    }
                }
            }

            SearchModel {
                id: searchModel
                inbox: Inbox

                onResultsChanged: {
                    if (searchResults.count == 0) {
                        searchResults.currentIndex = -1
                        emptyState.visible = true
                    } else {
                        searchResults.currentIndex = 0
                        emptyState.visible = false
                    }
                    searchResults.openCurrentItem()
                }
            }

            ListView {
                id: searchResults
                anchors {
                    left: parent.left
                    top: header.bottom
                    topMargin: _HEADER_CONTENT_SPACING
                    bottom: parent.bottom
                }
                width: 400 * Hdpi.FontScalingFactor
                clip: true
                model: searchModel
                interactive: true

                function openCurrentItem() {
                    if (currentItem) {
                        root.openMessage(currentItem.conversationId, currentItem.messageId)
                    } else {
                        root.openMessage(-1, -1)
                    }
                }

                onCurrentItemChanged: {
                    openCurrentItem()
                }

                delegate: SearchResult {
                    messageId: messageId_
                    conversationId: conversationId_
                    senderDisplayName: sender_
                    snippet: snippet_
                    date: date_
                    hasAttachments: hasAttachments_

                    onShowMessageInInboxRequested: root.showMessageInInbox(conversationId, messageId)
                }
            }

            ScrollView {
                anchors {
                    left: searchResults.right
                    leftMargin: 20 * Hdpi.FontScalingFactor
                    right: parent.right
                    top: header.bottom
                    topMargin: _HEADER_CONTENT_SPACING
                    bottom: parent.bottom
                }

                Column {
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                    id: singleMessageContainer

                    // first child is a MessageDelegate
                }
            }

            //Rectangle { color: "red"
            Item {
                id: emptyState
                anchors {
                    left: parent.left
                    right: parent.right
                    top: header.bottom
                    topMargin: _HEADER_CONTENT_SPACING
                    bottom: parent.bottom
                }
                visible: false

                Column {
                    anchors {
                        verticalCenter: parent.verticalCenter
                        left: parent.left
                        leftMargin: 50 * Hdpi.FontScalingFactor
                        right: parent.right
                        rightMargin: 50 * Hdpi.FontScalingFactor
                    }

                    NativeText {
                        anchors {
                            left: parent.left
                            right: parent.right
                        }
                        horizontalAlignment: Text.AlignHCenter
                        font.pointSize: Style.fontSize.loggingIn
                        text: searchInput.text.trim().length === 0
                              ? qsTr("Type to search")
                              : qsTr("No result for \"%1\"").arg(searchInput.text.trim())
                    }

                    NativeText {
                        anchors {
                            left: parent.left
                            right: parent.right
                        }
                        horizontalAlignment: Text.AlignHCenter
                        font.pointSize: Style.fontSize.big
                        visible: searchInput.text.trim().length !== 0
                                 && (!root.global || root.direction != "both")
                        textFormat: Text.StyledText
                        text: "<br>" + qsTr("Try searching <a href=\"searchEverywhere:\">everywhere</a>")
                        onLinkActivated: {
                            root.global = true
                            root.direction = "both"
                            root.updateResults()
                        }
                    }
                }
            }
        }
    }
}
