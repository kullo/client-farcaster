/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
import QtQuick 2.6

Item {
    default property alias children: column.children
    property int padding: 5 * Hdpi.FontScalingFactor
    property color backgroundColor: "#303030"
    width: 200 * Hdpi.FontScalingFactor

    z: 1

    id: root
    implicitHeight: main.implicitHeight + pointer.height

    Rectangle {
        id: main
        color: root.backgroundColor
        radius: 5 * Hdpi.FontScalingFactor

        anchors {
            left: parent.left
            right: parent.right
        }

        implicitHeight: padding + column.implicitHeight + padding

        Column {
            id: column

            padding: root.padding

            anchors {
                fill: parent
                margins: padding
            }

            spacing: 5 * Hdpi.FontScalingFactor
        }
    }

    Pointer {
        id: pointer
        color: root.backgroundColor
        size: 8 * Hdpi.FontScalingFactor
        anchors {
            top: main.bottom
            horizontalCenter: parent.horizontalCenter
        }
    }
}
