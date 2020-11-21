/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
import QtQuick 2.4

import "../"
import "../native"

NativeButton {
    text: qsTr("Back")
    style: KulloButtonStyle {
        source: "/resources/scalable/arrow-rounded-left.svg"
        iconPosition: _POSITION_LEFT
        paddingH: 0
        textColor: Style.black
        backgroundColor: "transparent"
        hoverColor: "#22000000"
    }
}
