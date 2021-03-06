/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
import QtQuick 2.4

NativeWindow {
    windowTypeModalWindow: true
    modality: Qt.WindowModal

    onVisibleChanged: {
        if (!visible)
        {
            // just closed widow
            console.log("Just closed NativeModalWindow")
            ModalityWatcher.closed(objectName)
        }
    }
}
