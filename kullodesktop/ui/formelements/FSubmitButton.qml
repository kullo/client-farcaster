/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
import QtQuick 2.4
import QtQuick.Controls 1.3

FButton {
    text: qsTr("Submit")

    onButtonAction: {
        var form = findParentForm()
        if (!form)
        {
            console.warn("Parent form not found.")
        }
        else
        {
            form.submit()
        }
    }
}
