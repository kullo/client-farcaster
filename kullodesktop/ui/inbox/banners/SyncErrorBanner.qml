/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
import QtQuick 2.4

import "../../"
import "../../native"

Rectangle {
    id: _root
    property bool showBanner: false
    anchors.horizontalCenter: parent.horizontalCenter
    width: Math.max(250, bannerText.contentWidth) + 2*15
    height: 40
    radius: 7
    color: Style.errorBannerBackground
    z: 3

    function show(errmsg) { bannerText.text = errmsg; showBanner = true }
    function hide() { showBanner = false }

    y: showBanner ? -radius : -height

    NativeText {
        id: bannerText
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            topMargin: 15
        }
        color: Style.errorBannerTextColor
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.NoWrap
    }

    Behavior on y {
        NumberAnimation {
            duration: 50
            easing.type: Easing.OutBounce
        }
    }
}
