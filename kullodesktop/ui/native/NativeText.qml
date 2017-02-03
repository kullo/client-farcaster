/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
import QtQuick 2.4

import ".."

Text {
    id: root
    // Set defaults
    textFormat: Text.PlainText
    renderType: Text.NativeRendering
    font.pointSize: Style.fontSize.normal
    wrapMode: Text.WrapAtWordBoundaryOrAnywhere

    SystemPalette {
        id: palette
        colorGroup: root.enabled ? SystemPalette.Active : SystemPalette.Disabled
    }

    color: palette.text
}
