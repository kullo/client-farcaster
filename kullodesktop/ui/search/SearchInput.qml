/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

TextField {
    /* public */
    signal searchRequested(var query)

    /* private */
    id: root

    // height 32, radius 4 like KulloButtonStyle

    implicitWidth: 200
    height: 32
    style: TextFieldStyle {
        background: Rectangle {
            radius: 4
            color: "#55ffffff"
        }
        renderType: Text.NativeRendering
    }
    onAccepted: root.searchRequested(text)
}
