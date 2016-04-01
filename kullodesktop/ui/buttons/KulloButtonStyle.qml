/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Controls.Styles 1.3
import Kullo.Visuals 1.0

import "../"
import "../native"

ButtonStyle {
    id: root
    property int _POSITION_LEFT: 1
    property int _POSITION_RIGHT: 2

    property int iconPosition: _POSITION_RIGHT
    property url source: ""
    property int paddingH: 14
    property int paddingV: 6
    property int borderRadius: 4
    property color backgroundColor: Style.buttonBackgroundColor
    property color backgroundColorDisabled: Style.buttonBackgroundColorDisabled
    property color hoverColor: Style.buttonHoverColor
    property color textColor: Style.buttonTextColor

    property int borderRadiusTopLeft: -1
    property int borderRadiusTopRight: -1
    property int borderRadiusBottomRight: -1
    property int borderRadiusBottomLeft: -1

    label: Item {}
    background: Item {
        /* public */
        property color backgroundColor: root.backgroundColor
        property color backgroundColorDisabled: root.backgroundColorDisabled
        property color hoverColor: root.hoverColor
        property color textColor: root.textColor
        property url iconSource: root.source
        property int iconPosition: root.iconPosition
        property int paddingH: root.paddingH
        property int paddingV: root.paddingV
        property int borderRadius: root.borderRadius
        property int borderRadiusTopLeft: root.borderRadiusTopLeft
        property int borderRadiusTopRight: root.borderRadiusTopRight
        property int borderRadiusBottomRight: root.borderRadiusBottomRight
        property int borderRadiusBottomLeft: root.borderRadiusBottomLeft

        /* private */
        property bool _hasLabel: control.text !== ""

        implicitWidth: background.implicitWidth
        implicitHeight: background.implicitHeight

        signal clicked();
        signal entered();
        signal exited();

        AdvancedRectangle {
            id: background
            color: !control.enabled ? backgroundColorDisabled : backgroundColor
            radius: borderRadius
            radiusTopLeft: borderRadiusTopLeft
            radiusTopRight: borderRadiusTopRight
            radiusBottomRight: borderRadiusBottomRight
            radiusBottomLeft: borderRadiusBottomLeft

            implicitHeight: (_hasLabel ? Math.max(icon.height, label.implicitHeight) : icon.height)
                            + 2*paddingV
            implicitWidth: _row.x + _row.implicitWidth + _row.x
            height: implicitHeight
            width: implicitWidth

            AdvancedRectangle {
                id: _hover
                anchors.fill: parent
                color: control.activeFocus ? hoverColor : "transparent"
                radius: background.radius
                radiusTopLeft: borderRadiusTopLeft
                radiusTopRight: borderRadiusTopRight
                radiusBottomRight: borderRadiusBottomRight
                radiusBottomLeft: borderRadiusBottomLeft

                states: [
                    State {
                        name: "basic"
                    },
                    State {
                        name: "hovered"
                        PropertyChanges {
                            target: _hover
                            color: hoverColor
                        }
                    }
                ]

                state: control.hovered ? "hovered" : "basic"

                Row {
                    id: _row
                    x: paddingH
                    y: paddingV
                    layoutDirection: iconPosition == root._POSITION_RIGHT
                                     ? Qt.LeftToRight
                                     : Qt.RightToLeft
                    spacing: 8

                    NativeText {
                        id: label
                        color: textColor
                        text: control.text
                        anchors {
                            verticalCenter: parent.verticalCenter
                        }
                    }

                    NativeImage {
                        id: icon
                        source: iconSource
                        width: (iconSource && iconSource != "") ? 20 : 0
                        height: 20
                        anchors {
                            verticalCenter: parent.verticalCenter
                        }
                    }
                }
            }
        }
    }
}
