/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
import QtQuick 2.4

TextEdit {
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
    selectionColor: palette.highlight
    selectedTextColor: palette.highlightedText
}
