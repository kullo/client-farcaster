/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
import QtQuick 2.6

Item {
    default property alias children: column.children
    property int padding: 5
    property color backgroundColor: "#303030"
    width: 200

    z: 1

    id: root
    implicitHeight: main.implicitHeight + pointer.height

    Rectangle {
        id: main
        color: root.backgroundColor
        radius: 5

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

            spacing: 5
        }
    }

    Pointer {
        id: pointer
        color: root.backgroundColor
        size: 8
        anchors {
            top: main.bottom
            horizontalCenter: parent.horizontalCenter
        }
    }
}
