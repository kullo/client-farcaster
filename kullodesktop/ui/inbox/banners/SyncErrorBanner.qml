/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
import QtQuick 2.4

import "../../"
import "../../native"

Rectangle {
    /* private */
    id: root
    property bool _showBanner: false
    property int _paddingV: 8
    property int _paddingH: 15
    anchors.horizontalCenter: parent.horizontalCenter
    width: Math.max(250, bannerText.contentWidth) + 2*_paddingH
    height: bannerText.implicitHeight
            + radius
            + 2*_paddingV
    radius: Style.bannerBorderRadius
    color: Style.errorBannerBackground
    z: 3

    function show(errmsg) { bannerText.text = errmsg; _showBanner = true }
    function hide() { _showBanner = false }

    y: _showBanner ? -radius : -height

    NativeText {
        id: bannerText
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            topMargin: radius + _paddingV
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
