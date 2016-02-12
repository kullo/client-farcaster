/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Controls.Styles 1.3
import Kullo.Visuals 1.0

import "../"
import "../native"

ButtonStyle {
    id: _style
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
        property color backgroundColor: _style.backgroundColor
        property color backgroundColorDisabled: _style.backgroundColorDisabled
        property color hoverColor: _style.hoverColor
        property color textColor: _style.textColor
        property url iconSource: _style.source
        property int iconPosition: _style.iconPosition
        property int paddingH: _style.paddingH
        property int paddingV: _style.paddingV
        property int borderRadius: _style.borderRadius
        property int borderRadiusTopLeft: _style.borderRadiusTopLeft
        property int borderRadiusTopRight: _style.borderRadiusTopRight
        property int borderRadiusBottomRight: _style.borderRadiusBottomRight
        property int borderRadiusBottomLeft: _style.borderRadiusBottomLeft

        /* private */
        property bool _hasLabel: control.text !== ""

        implicitWidth: _background.implicitWidth
        implicitHeight: _background.implicitHeight

        signal clicked();
        signal entered();
        signal exited();

        AdvancedRectangle {
            id: _background
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
                radius: _background.radius
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
                    layoutDirection: iconPosition == _style._POSITION_RIGHT
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
