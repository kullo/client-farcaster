/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
import QtQuick 2.4

import "../../"
import "../../native"

Rectangle {
    id: _root
    property bool showBanner: false
    property string progressText: ""
    anchors.horizontalCenter: parent.horizontalCenter
    width: 250
    height: 40
    radius: 7
    color: Style.bannerBackground
    z: 3

    function show() { showBanner = true }
    function hide() { showBanner = false }

    y: showBanner ? -radius : -height

    NativeText {
        id: syncingText
        anchors {
            top: parent.top
            left: parent.left
            topMargin: 15
        }
        Component.onCompleted: {
            anchors.leftMargin = (_root.width - syncingText.implicitWidth)/2
        }

        property int dotCount: 3
        text: qsTr("Syncing") + " "
              + ((progressText !== "") ? "(" + progressText + ") " : "")
              + "...".slice(0, dotCount);

        Timer {
            running: _root.showBanner
            interval: 800
            repeat: true
            triggeredOnStart: false
            onTriggered: {
                if (syncingText.dotCount < 3) syncingText.dotCount++;
                else syncingText.dotCount = 0;
            }
        }
    }

    Behavior on y {
        NumberAnimation {
            duration: 50
            easing.type: Easing.OutBounce
        }
    }
}
