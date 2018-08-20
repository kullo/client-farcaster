/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Window 2.2
import Kullo 1.0

Window {
    property bool windowTypeModalWindow: false

    SystemPalette {
        id: palette
        colorGroup: SystemPalette.Active
    }
    color: palette.window

    function _validate() {
        if (windowTypeModalWindow)
        {
            if (objectName.trim() === "")
            {
                console.warn("objectName should not be empty for NativeModalWindow.")
            }
        }
        else
        {
            if (modality != Qt.NonModal)
            {
                console.error("Modality in NativeWindow must not be changed. Use NativeModalWindow.")
            }
        }
    }

    function openWindow() {
        _validate()
        if (windowTypeModalWindow)
        {
            if (!ModalityWatcher.aboutToOpen(objectName))
            {
                // don't make window visible
                return
            }
        }
        visible = true
    }

    function closeWindow() {
        var closeEvent = { accepted: true };
        closing(closeEvent)
        visible = false
    }

    function handleNativeWindowShortcuts(event)
    {
        if (event.accepted) return false

        // CTRL + W (Win, OS X, KDE, Gnome)
        if ((event.modifiers & Qt.ControlModifier) && event.key === Qt.Key_W)
        {
            event.accepted = true
            closeWindow()
            return true
        }
        // CTRL + M (OS X)
        else if ((event.modifiers & Qt.ControlModifier) && event.key === Qt.Key_M)
        {
            if (Os.osx && !windowTypeModalWindow)
            {
                event.accepted = true
                showMinimized()
                return true
            }
        }

        return false
    }
}
