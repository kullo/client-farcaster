/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
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
    property int textPaddingTop: Os.osx ? 5 : -1

    /* private */
    property int _TEXT_PADDING_TOP_DEFAULT: 3

    color: "#f7f7f7"
    radius: 3
    border {
        color: "#ccc"
        width: 1
    }
    width: label.contentWidth
           + label.anchors.leftMargin
           + label.anchors.rightMargin
    height: 20

    NativeText {
        id: label
        anchors {
            left: parent.left
            top: parent.top
            topMargin: textPaddingTop >= 0 ? textPaddingTop : _TEXT_PADDING_TOP_DEFAULT
            leftMargin: 6
            rightMargin: 6
        }
        verticalAlignment: Text.AlignVCenter
        wrapMode: Text.NoWrap
        font.family: FontList.MonospaceFamily
        font.pointSize: Style.fontSize.shortcutOverlayKey
    }
}
