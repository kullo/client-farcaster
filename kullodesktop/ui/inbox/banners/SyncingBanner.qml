/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
import QtQuick 2.4

import "../../"
import "../../native"

Rectangle {
    id: _root

    property string progressText: ""
    /* private */
    property bool _showBanner: false
    property int _paddingV: 8

    anchors.horizontalCenter: parent.horizontalCenter
    width: 250
    height: syncingText.implicitHeight
            + radius
            + 2*_paddingV
    radius: Style.bannerBorderRadius
    color: Style.bannerBackground
    z: 3

    function show() { _showBanner = true }
    function hide() { _showBanner = false }

    y: _showBanner ? -radius : -height

    NativeText {
        id: syncingText
        anchors {
            top: parent.top
            left: parent.left
            topMargin: radius + _paddingV
        }
        Component.onCompleted: {
            anchors.leftMargin = (_root.width - syncingText.implicitWidth)/2
        }

        property int dotCount: 3
        text: qsTr("Syncing") + " "
              + ((progressText !== "") ? "(" + progressText + ") " : "")
              + "...".slice(0, dotCount);

        Timer {
            running: _showBanner
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
