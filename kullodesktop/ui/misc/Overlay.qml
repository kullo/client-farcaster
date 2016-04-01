/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
import QtQuick 2.4

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
    width: 150
    height: 150
    visible: true
    radius: 15
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

    Timer {
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
        font.pointSize: Style.fontSize.overlay
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
