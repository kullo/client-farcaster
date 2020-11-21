/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
import QtQuick 2.3
import Kullo.Visuals 1.0

import "../"
import "../native"

FocusScope {
    /* private */
    property int _RADIUS: 4 * Hdpi.FontScalingFactor
    property int _BORDER_WIDTH: 0
    property color _BORDER_COLOR: Style.gray
    property color _BACKGROUND_COLOR: Style.messagesListBackground
    property color _FOREGROUND_COLOR: Style.black
    property int _ELEMENT_SIZE: 28 * Hdpi.FontScalingFactor
    property int _ADJUST_X: 2 * Hdpi.FontScalingFactor

    id: root
    height: 8*_ELEMENT_SIZE + 2*_BORDER_WIDTH
    width: 15*_ELEMENT_SIZE + 2*_BORDER_WIDTH

    visible: false

    onActiveFocusChanged: if (!activeFocus) close()

    function placeBelow(x, y) {
        root.x = (x < width)
                ? -5 * Hdpi.FontScalingFactor
                : x-width + 15 * Hdpi.FontScalingFactor
        root.y = y + 8 * Hdpi.FontScalingFactor
        pointer.position = pointer.topEdge
        pointer.pointToX = x
        pointer.pointToY = y
    }

    function placeAbove(x, y) {
        root.x = (x < width)
                ? -5 * Hdpi.FontScalingFactor
                : x-width + 15 * Hdpi.FontScalingFactor
        root.y = (y-height) - 6 * Hdpi.FontScalingFactor
        pointer.position = pointer.bottomEdge
        pointer.pointToX = x
        pointer.pointToY = y
    }

    function open() {
        visible = true
        forceActiveFocus()
    }

    function close() {
        visible = false
    }

    signal selected(string emoji)
    signal canceled()

    Keys.onEscapePressed: canceled()

    MouseArea {
        anchors {
            fill: parent
        }

        hoverEnabled: true
        onClicked: root.forceActiveFocus()

        Rectangle {
            anchors {
                fill: parent
            }
            radius: root._RADIUS
            border.width: _BORDER_WIDTH
            border.color: _BORDER_COLOR

            color: _BACKGROUND_COLOR

            Item {
                id: pointer
                property int topEdge: 1
                property int bottomEdge: -1
                property int position: topEdge
                property real pointToX: 0
                property real pointToY: 0
                height: 10 * Hdpi.FontScalingFactor
                width: 5 * Hdpi.FontScalingFactor
                x: (-root.x) + pointToX + _ADJUST_X - (width/2.0)
                y: position == topEdge ? -height : parent.height

                AdvancedRectangle {
                    color: _BACKGROUND_COLOR
                    anchors.fill: parent
                    radiusTopLeft:  parent.position === parent.topEdge ? 3 * Hdpi.FontScalingFactor : 0
                    radiusTopRight: parent.position === parent.topEdge ? 3 * Hdpi.FontScalingFactor : 0
                    radiusBottomLeft:  parent.position === parent.bottomEdge ? 3 * Hdpi.FontScalingFactor : 0
                    radiusBottomRight: parent.position === parent.bottomEdge ? 3 * Hdpi.FontScalingFactor : 0
                }
            }

            Item {
                id: content
                anchors {
                    fill: parent
                    margins: root._BORDER_WIDTH
                }

                Text {
                    anchors {
                        top: parent.top
                        left: parent.left

                    }
                    id: previewEmoji
                    width:  5*_ELEMENT_SIZE
                    height: 5*_ELEMENT_SIZE
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: 2.5*_ELEMENT_SIZE
                    color: _FOREGROUND_COLOR
                    text: grid.currentItem ? grid.currentItem.emoji : ""
                }

                NativeText {
                    property int _PADDING_H: 10 * Hdpi.FontScalingFactor
                    anchors {
                        top: previewEmoji.bottom
                        left: parent.left
                        leftMargin: _PADDING_H
                    }
                    id: previewDescription
                    width:  5*_ELEMENT_SIZE - 2*_PADDING_H
                    height: 2*_ELEMENT_SIZE
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignTop
                    font.pointSize: Style.fontSize.small
                    color: _FOREGROUND_COLOR
                    text: grid.currentItem ? grid.currentItem.description : ""
                }

                GridView {
                    id: grid
                    focus: true
                    anchors {
                        left: previewEmoji.right
                        right: parent.right
                        top: parent.top
                        bottom: parent.bottom
                    }

                    cellWidth: root._ELEMENT_SIZE
                    cellHeight: root._ELEMENT_SIZE

                    model: EmojiList {}

                    Keys.onReturnPressed: selected(currentItem.emoji)
                    Keys.onEnterPressed: selected(currentItem.emoji)
                    Keys.onPressed: {
                        if (event.modifiers !== 0) return;

                        var cols = 10

                        if (event.key === Qt.Key_Home) {
                            var newIndexBegin = Math.floor(currentIndex/cols) * cols
                            currentIndex = newIndexBegin
                            event.accepted = true
                            return
                        }

                        if (event.key === Qt.Key_End) {
                            var newIndexEnd = (Math.floor(currentIndex/cols) * cols) + (cols-1)
                            newIndexEnd = Math.min(newIndexEnd, model.count-1)
                            currentIndex = newIndexEnd
                            event.accepted = true
                            return
                        }
                    }

                    delegate: TooltipArea {
                        /* public */
                        property string emoji: emoji_
                        property string description: description_

                        anchors.fill: undefined
                        height: root._ELEMENT_SIZE
                        width: root._ELEMENT_SIZE

                        text: description_

                        MouseArea {
                            anchors.fill: parent

                            onClicked: {
                                grid.forceActiveFocus()
                                grid.currentIndex = index
                            }
                            onDoubleClicked: {
                                grid.forceActiveFocus()
                                selected(emoji_)
                            }

                            Text {
                                id: label
                                text: emoji_
                                anchors.fill: parent
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                color: _FOREGROUND_COLOR
                                font.pixelSize: 0.60*_ELEMENT_SIZE
                            }
                        }
                    }

                    highlight: Rectangle {
                        radius: 2 * Hdpi.FontScalingFactor
                        color: "#55000000"
                    }
                    highlightMoveDuration: 20
                }
            }
        }
    }
}
