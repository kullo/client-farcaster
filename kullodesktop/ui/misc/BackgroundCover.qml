/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
import QtQuick 2.6

Rectangle {
    signal fadedIn()

    id: root
    /* status banners are at z=3 */
    z: 4
    anchors.fill: parent
    color: "#99000000"

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
        root.fadedIn()
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
        acceptedButtons: Qt.AllButtons
        onClicked: {
            fadeOut()
        }

        // Stop all scroll events from falling through overlay
        scrollGestureEnabled: true
        onWheel: { wheel.accepted = true }
    }
}
