/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
import QtQuick 2.4

import "../../"
import "../../native"

Rectangle {
    id: _root
    /* private */
    property bool _showBanner: false
    property int _paddingV: 8
    property int _paddingH: 15
    anchors.horizontalCenter: parent.horizontalCenter
    width: Math.max(parent.width*0.4, 400)
    radius: Style.bannerBorderRadius
    height: syncingText.implicitHeight
            + syncingText.anchors.topMargin
            + syncingText.anchors.bottomMargin
    color: Style.errorBannerBackground
    z: 3

    function show() { _showBanner = true }
    function hide() { _showBanner = false }

    y: _showBanner ? -radius : -height

    NativeTextWithLinks {
        id: syncingText
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            leftMargin: _paddingH
            rightMargin: _paddingH
            topMargin: radius + _paddingV
            bottomMargin: _paddingV
        }
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        color: Style.errorBannerTextColor
        html: qsTr("This software is too old to sync with the server.") + " " +
              qsTr("Please download the current version from " +
                   "<a style='color: %1' href='%2'>kullo.net/download</a>")
              .arg(Style.errorBannerTextColor)
              .arg(KulloVersionChecker.updateUrl)
    }

    Behavior on y {
        NumberAnimation {
            duration: 50
            easing.type: Easing.OutBounce
        }
    }
}
