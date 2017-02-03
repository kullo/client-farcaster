/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Controls 1.3

FButton {
    text: qsTr("Clear")

    onButtonAction: {
        var form = findParentForm()
        if (!form)
        {
            console.warn("Parent form not found.")
        }
        else
        {
            form.clear()
            form.resetFocus()
        }
    }
}
