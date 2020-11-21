/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
import QtQuick 2.4

import ".."

Text {
    id: root
    // Set defaults
    textFormat: Text.PlainText
    renderType: Text.NativeRendering
    font.pointSize: Style.fontSize.normal
    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
    linkColor: color

    SystemPalette {
        id: palette
        colorGroup: root.enabled ? SystemPalette.Active : SystemPalette.Disabled
    }

    color: palette.text
}
