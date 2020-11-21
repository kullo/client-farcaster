/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
import QtQuick 2.4

Item {
    /* public */
    property alias color: content.color
    property real size: 10 * Hdpi.FontScalingFactor

    /* private */
    id: root
    width: 2*size
    height: size

    clip: true

    Rectangle {
        id: content
        property real sideLength: root.width/Math.SQRT2
        property real rotatedHeight: sideLength/Math.SQRT2
        width: sideLength
        height: sideLength

        anchors {
            top: parent.top
            topMargin: -(sideLength/2)
            horizontalCenter: parent.horizontalCenter
        }

        transform: Rotation {
            origin.x: content.width/2
            origin.y: content.height/2
            angle: 45
        }
    }
}
