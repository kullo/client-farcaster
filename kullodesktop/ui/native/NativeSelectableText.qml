/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
import QtQuick 2.4

import ".."

NativeTextEdit {
    id: root
    height: implicitHeight

    textFormat: Text.PlainText
    readOnly: true
    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
    font.pointSize: Style.fontSize.normal
}
