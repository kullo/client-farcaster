/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
import QtQuick 2.4

import ".."
import "../native"

//kbd {
//    background-color: #f7f7f7;
//    border: 1px solid #ccc;
//    border-radius: 3px;
//    box-shadow: 0 1px 0 rgba(0, 0, 0, 0.2), 0 0 0 2px #fff inset;
//    color: #333;
//    display: inline-block;
//    font-family: Arial,Helvetica,sans-serif;
//    font-size: 11px;
//    line-height: 1.4;
//    margin: 0 0.1em;
//    padding: 0.1em 0.6em;
//    text-shadow: 0 1px 0 #fff;
//    white-space: nowrap;
//}

Rectangle {
    /* public */
    property alias text: label.text
    property alias fontFamily: label.font.family
    // a negative value means unset
    property int textPaddingTop: Os.osx
                                 ? 5 * Hdpi.FontScalingFactor
                                 : -1

    /* private */
    property int _TEXT_PADDING_TOP_DEFAULT: 3 * Hdpi.FontScalingFactor

    color: "#f7f7f7"
    radius: 3 * Hdpi.FontScalingFactor
    border {
        color: "#ccc"
        width: 1 * Hdpi.FontScalingFactor
    }
    width: label.contentWidth
           + label.anchors.leftMargin
           + label.anchors.rightMargin
    height: 20 * Hdpi.FontScalingFactor

    NativeText {
        id: label
        anchors {
            left: parent.left
            top: parent.top
            topMargin: textPaddingTop >= 0 ? textPaddingTop : _TEXT_PADDING_TOP_DEFAULT
            leftMargin: 6 * Hdpi.FontScalingFactor
            rightMargin: 6 * Hdpi.FontScalingFactor
        }
        verticalAlignment: Text.AlignVCenter
        wrapMode: Text.NoWrap
        font.family: FontList.MonospaceFamily
        font.pointSize: Style.fontSize.shortcutOverlayKey
    }
}
