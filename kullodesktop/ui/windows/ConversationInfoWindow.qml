/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import Kullo 1.0

import "../buttons"
import "../misc"
import "../native"
import "../js/js_helpers.js" as JsHelpers

NativeWindow {
    id: root
    title: qsTr("Conversation info")

    /* public */
    property int conversationId: -1
    property var participantNames // adress->name map

    /* private */
    property int _DEFAULT_WINDOW_WIDTH: 550 * Hdpi.FontScalingFactor
    property int _DEFAULT_WINDOW_HEIGHT: 250 * Hdpi.FontScalingFactor
    property Conversation _conversation: conversationId !== -1
                                         ? Inbox.allConversations.get(conversationId)
                                         : null
    property var _selectedParticipants: [] // array of addresses

    signal startConversationWithParticipants(var participants)

    width: _DEFAULT_WINDOW_WIDTH
    height: _DEFAULT_WINDOW_HEIGHT

    onVisibleChanged: {
        if (visible) {
            // reset window dimensions
            root.width = _DEFAULT_WINDOW_WIDTH
            root.height = _DEFAULT_WINDOW_HEIGHT

            mainItem.forceActiveFocus()
        }
    }

    onParticipantNamesChanged: {
        participantsModel.clear()
        for (var address in participantNames) {
            var name = participantNames[address];
            participantsModel.append({ "address_": address, "name_": name })
        }

        _selectedParticipants = []
    }

    on_SelectedParticipantsChanged: {
        actionButton.enabled = (_selectedParticipants.length > 0)
    }

    // Main Qt Quick Item required for attached property `Keys`
    Item {
        id: mainItem
        anchors {
            fill: parent
            margins: 10 * Hdpi.FontScalingFactor
        }

        Keys.onEscapePressed: root.closeWindow()
        Keys.onPressed: handleNativeWindowShortcuts(event)

        Item {
            id: leftColumn
            anchors {
                top: parent.top
                left: parent.left
                bottom: parent.bottom
            }
            width: 300 * Hdpi.FontScalingFactor

            ListModel {
                id: participantsModel
            }

            ScrollView {
                id: scrollView
                anchors {
                    left: parent.left
                    right: parent.right
                    top: parent.top
                    bottom: actionButton.top
                    bottomMargin: 10 * Hdpi.FontScalingFactor
                }
                width: 300 * Hdpi.FontScalingFactor
                clip: true

                ListView {
                    id: stagedAddresses
                    anchors.fill: parent
                    interactive: false
                    model: participantsModel

                    delegate: Item {
                        height: 44 * Hdpi.FontScalingFactor
                        width: scrollView.width

                        property string address: address_
                        property string name: name_
                        property alias checked: checkbox.checked

                        NativeImage {
                            id: participantAvatar
                            asynchronous: true
                            source: "image://participantavatars/" + Utils.urlencode(address)

                            width: 40 * Hdpi.FontScalingFactor
                            height: 40 * Hdpi.FontScalingFactor

                            anchors {
                                left: parent.left
                                top: parent.top
                                topMargin: 2 * Hdpi.FontScalingFactor
                            }
                        }

                        NativeSelectableText {
                            anchors {
                                left: participantAvatar.right
                                leftMargin: 8 * Hdpi.FontScalingFactor
                                top: parent.top
                                bottom: parent.bottom
                                right: checkbox.left
                            }
                            text: ((name.length !== 0) ? name + "\n" : "")
                                  + address
                            verticalAlignment: Text.AlignVCenter
                        }

                        CheckBox {
                            id: checkbox
                            anchors {
                                right: parent.right
                                verticalCenter: parent.verticalCenter
                            }
                            width: 40 * Hdpi.FontScalingFactor
                            onCheckedChanged: {
                                if (checked) {
                                    // remove first to ensure uniqueness
                                    JsHelpers.removeFromArray(_selectedParticipants, address)
                                    _selectedParticipants.push(address)
                                } else {
                                    JsHelpers.removeFromArray(_selectedParticipants, address)
                                }
                                root._selectedParticipantsChanged()
                            }
                        }
                    }
                }
            }

            NativeButton {
                id: actionButton
                text: qsTr("Create new conversation")
                tooltip: qsTr("Create new conversation with selected participants")
                style: KulloButtonStyle {}
                anchors {
                    left: parent.left
                    bottom: parent.bottom
                }
                onClicked: {
                    root.closeWindow()
                    root.startConversationWithParticipants(_selectedParticipants)
                }
            }
        }

        Column {
            id: rightColumn
            anchors {
                top: parent.top
                left: leftColumn.right
                leftMargin: 20 * Hdpi.FontScalingFactor
                right: parent.right
            }
            spacing: 10 * Hdpi.FontScalingFactor

            NativeText {
                text: qsTr("Statistics")
                font.bold: true
            }

            Row {
                anchors {
                    left: parent.left
                    right: parent.right
                }
                spacing: 10 * Hdpi.FontScalingFactor

                NativeText {
                    id: statsLabel
                    text: qsTr("Messages") + ":"
                }

                NativeSelectableText {
                    TooltipArea {
                        text: qsTr("incoming")
                    }
                    text: "%1↓".arg(_conversation ? _conversation.countIncoming : -1)
                }

                NativeSelectableText {
                    TooltipArea {
                        text: qsTr("outgoing")
                    }
                    text: "%1↑".arg(_conversation ? _conversation.countOutgoing : -1)
                }
            }
        }
    }
}
