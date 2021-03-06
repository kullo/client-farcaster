/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
import QtQuick 2.4
import QtQuick.Controls 1.3

Button {
    property bool _FORM_TYPE_IGNORE_IN_FORM_VALIDATION: true
    property bool _FORM_TYPE_IGNORE_IN_FORM_CLEARING: true

    signal buttonAction()

    onClicked: buttonAction()
    Keys.onReturnPressed: if (activeFocus) buttonAction()
    Keys.onEnterPressed: if (activeFocus) buttonAction()

    function findParentForm()
    {
        var elem = parent
        do {
            if (elem._FORM_TYPE_FORM)
            {
                return elem
            }
            elem = elem.parent
        } while (elem)
        return null
    }
}
