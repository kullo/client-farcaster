/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
import QtQuick 2.4

import ".."

NativeTextEdit {
    id: root
    height: implicitHeight

    readOnly: true
    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
    font.pointSize: Style.fontSize.normal
}
