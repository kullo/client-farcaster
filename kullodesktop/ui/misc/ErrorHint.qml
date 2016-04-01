/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import Kullo.Visuals 1.0

import ".."
import "../native"

Hint {
    id: root
    property string text
    property int maxWidth: -1

    visible: text != ""
    width: maxWidth === -1
           ? errorHintTextBox.implicitWidth
           : Math.min(errorHintTextBox.implicitWidth, maxWidth)
    height: visible
            ? 11+errorHintTextBox.implicitHeight
            : 0
    paddingTop: 11
    direction: "topright"
    color: Style.errorBannerBackground
    clip: true

    Item {
        id: errorHintTextBox
        implicitWidth: errorHintText.implicitWidth
                       + errorHintText.anchors.leftMargin
                       + errorHintText.anchors.rightMargin
        implicitHeight: errorHintText.contentHeight
                        + 2*errorHintText.anchors.topMargin
        height: implicitHeight

        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            topMargin: root.paddingTop
        }

        NativeText {
            id: errorHintText
            anchors {
                left: parent.left
                leftMargin: 10
                right: parent.right
                rightMargin: 10
                top: parent.top
                topMargin: 4
            }
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            color: Style.errorBannerTextColor
            text: root.text
        }
    }
}
