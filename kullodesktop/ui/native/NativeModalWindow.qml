/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
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
