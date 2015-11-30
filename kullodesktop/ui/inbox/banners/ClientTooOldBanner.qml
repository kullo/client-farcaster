/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
import QtQuick 2.4

import "../../"
import "../../native"

Rectangle {
    id: _root
    property bool showBanner: false
    anchors.horizontalCenter: parent.horizontalCenter
    width: 400
    radius: 7
    height: syncingText.implicitHeight
            + syncingText.anchors.topMargin
            + syncingText.anchors.bottomMargin
    color: Style.errorBannerBackground
    z: 3

    function show() { showBanner = true }
    function hide() { showBanner = false }

    y: showBanner ? -radius : -height

    NativeText {
        id: syncingText
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            leftMargin: 2*_root.radius
            rightMargin: 2*_root.radius
            topMargin: 2*_root.radius
            bottomMargin: 1.5*_root.radius
        }
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        color: Style.errorBannerTextColor
        textFormat: Text.RichText

        text: qsTr("This software is too old to sync with the server.") + " " +
              qsTr("Please download the current version from " +
                   "<a style='color: %1' href='%2'>kullo.net/download</a>")
              .arg(Style.errorBannerTextColor)
              .arg(KulloVersionChecker.updateUrl)
        onLinkActivated: Qt.openUrlExternally(link)
    }

    Behavior on y {
        NumberAnimation {
            duration: 50
            easing.type: Easing.OutBounce
        }
    }
}
