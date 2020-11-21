/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
import QtQuick 2.4
import QtQuick.Layouts 1.2
import Kullo 1.0

import "../"
import "../misc"
import "../native"

import "../js/js_helpers.js" as JSHelpers

Rectangle {
    /* public */
    property Conversation conversation: null

    /* private */
    property int _participantsCount: conversation
                                     ? JSHelpers.sizeOfMapObject(conversation.participantNames)
                                     : 0

    function rebuildReceiversList()
    {
        if (conversation)
        {
            var participantNames = conversation.participantNames
            participantsModel.clear()
            for (var address in participantNames) {
                participantsModel.append({ address: address, name: participantNames[address] });
            }
        }
    }

    // conversation switched
    onConversationChanged: rebuildReceiversList()

    // currently selected conversation changed
    Connections {
        target: conversation
        onCountChanged: rebuildReceiversList()
    }

    id: root
    color: Style.answerHeaderBackground
    anchors {
        top: parent.top
        left: parent.left
        right: parent.right
    }
    height: receiversGrid.height

    GridLayout {
        id: receiversGrid

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }

        columns: _participantsCount === 1 ? 1
                                          : _participantsCount === 2 ? 2
                                                                     : width > 550 * Hdpi.FontScalingFactor ? 3
                                                                                                            : 2

        property int _cellHeight: 10 * Hdpi.FontScalingFactor // max. of cells' implicit height

        columnSpacing: 10 * Hdpi.FontScalingFactor
        rowSpacing: 10 * Hdpi.FontScalingFactor

        Repeater {
            model: ListModel {
                id: participantsModel
            }

            //Rectangle { color: "blue"
            Item {
                id: cell
                Layout.preferredHeight: receiversGrid._cellHeight
                Layout.preferredWidth: (receiversGrid.width - receiversGrid.columnSpacing*(receiversGrid.columns-1)) / receiversGrid.columns

                implicitHeight: cellContent.implicitHeight

                onImplicitHeightChanged: {
                    receiversGrid._cellHeight = Math.max(receiversGrid._cellHeight,
                                                          implicitHeight)
                }

                //Rectangle { color: "green"
                Item {
                    id: cellContent
                    anchors {
                        left: parent.left
                        right: parent.right
                        verticalCenter: parent.verticalCenter
                    }
                    implicitHeight: Math.max(textbox.implicitHeight,
                                             participantAvatar.height)

                    NativeImage {
                        id: participantAvatar
                        anchors {
                            left: parent.left
                            verticalCenter: parent.verticalCenter
                        }
                        width: 40 * Hdpi.FontScalingFactor
                        height: 40 * Hdpi.FontScalingFactor
                        asynchronous: true
                        source: "image://participantavatars/" + Utils.urlencode(address)
                    }

                    //Rectangle { color: "red"
                    Item {
                        id: textbox
                        anchors {
                            left: participantAvatar.right
                            leftMargin: 8 * Hdpi.FontScalingFactor
                            right: parent.right
                            verticalCenter: parent.verticalCenter
                        }

                        property bool _addressOnly: !name || name === ""

                        implicitHeight: _addressOnly
                                        ? textboxAddressOnly.implicitHeight
                                        : textboxNameAndAddress.implicitHeight

                        NativeText {
                            id: textboxAddressOnly
                            anchors {
                                left: parent.left
                                right: parent.right
                                verticalCenter: parent.verticalCenter
                            }
                            wrapMode: Text.NoWrap
                            elide: Text.ElideMiddle
                            color: Style.answerHeaderForeground
                            font.pointSize: Style.fontSize.messageHeaderPrimary
                            text: address
                            visible: textbox._addressOnly

                            TooltipArea {
                                text: address
                            }
                        }

                        Column {
                            id: textboxNameAndAddress
                            visible: !textbox._addressOnly
                            anchors {
                                left: parent.left
                                right: parent.right
                                top: parent.top
                            }

                            NativeText {
                                id: textboxRow1
                                anchors {
                                    left: parent.left
                                    right: parent.right
                                }
                                wrapMode: Text.NoWrap
                                elide: Text.ElideRight
                                color: Style.answerHeaderForeground
                                font.pointSize: Style.fontSize.messageHeaderPrimary
                                text: name

                                TooltipArea {
                                    text: name
                                }
                            }

                            NativeText {
                                id: textboxRow2
                                anchors {
                                    left: parent.left
                                    right: parent.right
                                }
                                wrapMode: Text.NoWrap
                                elide: Text.ElideMiddle
                                color: Style.answerHeaderForegroundSecondary
                                font.pointSize: Style.fontSize.messageHeaderSecondary
                                text: address

                                TooltipArea {
                                    text: address
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
