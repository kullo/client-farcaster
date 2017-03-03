/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Controls.Styles 1.3
import Kullo.Visuals 1.0

import "../"
import "../native"

ButtonStyle {
    id: root
    property int _POSITION_LEFT: 1
    property int _POSITION_RIGHT: 2

    property int iconSize: 20
    property int iconPosition: _POSITION_RIGHT
    property url source: ""
    property int paddingH: 14
    property int paddingV: 6
    property int borderRadius: 4
    property color backgroundColor: Style.buttonBackgroundColor
    property color backgroundColorDisabled: Style.buttonBackgroundColorDisabled
    property color hoverColor: Style.buttonHoverColor
    property color textColor: Style.buttonTextColor

    property bool badgeEnabled: false

    property color progressBarColor: textColor
    property color progressBarBorderColor:"#55000000"
    property color progressBarBackgroundColor: Style.white

    property real progressMax: 100
    property real progress: -1

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
            implicitWidth: paddingH + row.implicitWidth + paddingH
            height: control.height
            width: control.width

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

                //Rectangle { color: "green"
                Item {
                    id: row
                    x: paddingH
                    y: paddingV
                    width: background.width - 2*paddingH
                    height: background.height - 2*paddingV
                    implicitWidth: label.implicitWidth
                                   + (icon.width ? labelIconSpacing + icon.width : 0)
                    implicitHeight: Math.max(labelBlock.implicitHeight, icon.height)

                    property real labelIconSpacing: label.implicitWidth > 0 ? 8 : 0
                    property real spaceLeft: width - (icon.width ? labelIconSpacing + icon.width : 0)

                    Column {
                        id: labelBlock

                        anchors {
                            left: iconPosition == root._POSITION_RIGHT ? parent.left : undefined
                            right: iconPosition == root._POSITION_LEFT ? parent.right : undefined
                            verticalCenter: parent.verticalCenter
                        }
                        width: row.spaceLeft

                        NativeText {
                            id: label
                            color: textColor
                            text: control.text
                            width: implicitWidth > 0 ? row.spaceLeft : 0

                            wrapMode: Text.NoWrap
                            elide: Text.ElideRight
                        }

                        spacing: 2

                        Rectangle {
                            id: progress
                            color: root.progressBarBackgroundColor
                            property real visibleValue: Math.min(
                                                            root.progress/root.progressMax,
                                                            1.0)
                            property bool enabled: visibleValue >= 0
                            width: labelBlock.width

                            implicitHeight: enabled ? 6 : 0
                            visible: enabled

                            border.width: 1
                            border.color: root.progressBarBorderColor

                            Rectangle {
                                color: root.progressBarColor
                                x: 2
                                y: 2
                                height: 2
                                width: (parent.width - 2*x) * progress.visibleValue
                            }
                        }
                    }

                    NativeImage {
                        id: icon
                        source: iconSource
                        width: (iconSource && iconSource != "") ? iconSize : 0
                        height: iconSize
                        smooth: true
                        anchors {
                            left: iconPosition == root._POSITION_RIGHT ? labelBlock.right : undefined
                            leftMargin: iconPosition == root._POSITION_RIGHT ? row.labelIconSpacing : 0

                            right: iconPosition == root._POSITION_LEFT ? labelBlock.left : undefined
                            rightMargin: iconPosition == root._POSITION_LEFT ? row.labelIconSpacing : 0

                            verticalCenter: parent.verticalCenter
                        }

                        Rectangle {
                            visible: root.badgeEnabled
                            color: Style.orange
                            anchors {
                                top: parent.top
                                right: parent.right
                                topMargin: -3
                                rightMargin: -3
                            }
                            width: 8
                            height: 8
                            radius: 8/2
                        }
                    }
                }
            }
        }
    }
}
