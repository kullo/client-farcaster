/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Controls 1.3
import Kullo 1.0

Button {
    Keys.onPressed: {
        if (!Os.osx
                && (event.key === Qt.Key_Enter || event.key === Qt.Key_Return)
                && event.modifiers === Qt.NoModifier)
        {
            event.accepted = true
            clicked()
        }
    }
}
