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
    id: root

    style: KulloButtonStyle {
        iconPosition: _POSITION_LEFT
        paddingH: paddingV
        source: root.iconSource
        textColor: Style.gray
        backgroundColor: "transparent"
        hoverColor: "#55ffffff"
    }
}
