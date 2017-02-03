/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
import QtQuick 2.4

Rectangle {
    id: root
    /* status banners are at z=3 */
    z: 4
    anchors.fill: parent
    color: "#77000000"

    state: "hidden"
    opacity: 0
    visible: opacity != 0

    function fadeOut() {
        root.state = "hidden"
        inboxContent.forceActiveFocus()
    }
    function fadeIn() {
        root.state = "visible"
        root.forceActiveFocus()
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

    transitions: Transition {
        NumberAnimation {
            target: root;
            properties: "opacity"
            easing.type: Easing.InCubic;
            duration: 150
        }
    }

    Keys.onEscapePressed: {
        event.accepted = true
        fadeOut()
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onClicked: {
            fadeOut()
        }
    }
}
