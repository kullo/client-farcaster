/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
.pragma library
.import QtQuick 2.4 as JSQtQuick

function createComponentOrThrow(url) {
    var component = Qt.createComponent(url);

    if(component.status !== JSQtQuick.Component.Ready)
    {
        if(component.status === JSQtQuick.Component.Error)
        {
            var errorString1 = "Error: " + component.errorString()
            console.error(errorString1);
            throw errorString1
        }
        else
        {
            var errorString2 = "Component not ready but not in Component.Error status"
            console.error(errorString2)
            throw errorString2
        }
    }
    return component
}
