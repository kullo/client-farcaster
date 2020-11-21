/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
import QtQuick 2.4
import Kullo 1.0

import ".."
import "../native"

Rectangle {
    /* public */
    property string text
    property bool active

    function show() {
        root.state = "visible"
        timer.restart()
    }

    /* private */
    id: root
    width: 150 * Hdpi.FontScalingFactor
    height: 150 * Hdpi.FontScalingFactor
    visible: true
    radius: 15 * Hdpi.FontScalingFactor
    opacity: 1
    color: "#dd000000"
    state: "hidden"

    function _fadeOut() {
        root.state = "hidden"
    }

    states: [
        State {
            name: "visible"
            PropertyChanges { target: root; opacity: 1 }
        },
        State {
            name: "hidden"
            PropertyChanges { target: root; opacity: 0 }
        }
    ]

    transitions: [
        Transition {
            from: "visible"
            to: "hidden"
            NumberAnimation {
                target: root;
                properties: "opacity";
                easing.type: Easing.InCubic;
                duration: 150
            }
        }
    ]

    StableTimer {
        id: timer
        interval: 800
        onTriggered: _fadeOut()
    }

    NativeText {
        anchors {
            fill: parent
            margins: 10
        }
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.pointSize: Style.fontSize.toast
        text: root.text
        // if this text wraps, layout is destroyed entirely
        wrapMode: Text.NoWrap
        color: "white"
    }

    Rectangle {
        id: bar
        anchors {
            top: parent.top
            horizontalCenter: parent.horizontalCenter
        }
        visible: !active
        height: parent.height
        width: parent.height * 5.0/100
        color: "white"
        radius: width * 0.3

        transform: Rotation {
            origin {
                x: bar.width / 2
                y: bar.height / 2
            }
            axis { x: 0; y: 0; z: 1 }
            angle: 45
        }
    }
}
