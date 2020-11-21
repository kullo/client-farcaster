/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
import QtQuick 2.4

import "../../"
import "../../native"
import "../../misc"

Rectangle {
    /* public */
    property string text: ""
    property alias progressIndeterminate: progressBar.indeterminate
    property alias progressValue: progressBar.value

    function reset() {
        text = qsTr("Syncing")
        progressIndeterminate = true
        progressValue = 0
    }

    /* private */
    id: root
    property bool _showBanner: false
    property int _paddingV: 8 * Hdpi.FontScalingFactor

    anchors.horizontalCenter: parent.horizontalCenter
    width: 250 * Hdpi.FontScalingFactor
    height: syncingText.implicitHeight
            + radius
            + 2*_paddingV
    radius: Style.bannerBorderRadius
    color: Style.bannerBackground
    z: 3

    function show() { _showBanner = true }
    function hide() { _showBanner = false }

    y: _showBanner ? -radius : -height

    Row {
        id: content
        anchors {
            top: parent.top
            topMargin: radius + _paddingV
            horizontalCenter: parent.horizontalCenter
        }
        spacing: 8 * Hdpi.FontScalingFactor

        CircularProgressBar {
            id: progressBar
            visible: _showBanner
            height: 16 * Hdpi.FontScalingFactor
            width: 16 * Hdpi.FontScalingFactor
            value: 0.75
            indeterminate: true
            color: syncingText.color
        }

        NativeText {
            id: syncingText
            text: root.text + " …"
        }
    }

    Behavior on y {
        NumberAnimation {
            duration: 50
            easing.type: Easing.OutBounce
        }
    }
}
