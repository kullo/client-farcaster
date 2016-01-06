/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
import QtQuick 2.4

TextInput {
    id: root
    renderType: Text.NativeRendering
    selectByMouse: true
    clip: true
    wrapMode: TextEdit.WordWrap

    SystemPalette {
        id: palette
        colorGroup: root.enabled ? SystemPalette.Active : SystemPalette.Disabled
    }

    color: palette.text
}
