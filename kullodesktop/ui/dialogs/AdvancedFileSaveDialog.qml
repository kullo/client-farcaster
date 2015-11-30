/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Dialogs 1.2
import Kullo 1.0

Item {
    id: _root
    property string title
    property string filename
    property url fileUrl

    function openDialog() {
        if (nativeDialog.valid) nativeDialog.open()
        else _root.openRequested()
    }

    function closeDialog() {
        if (nativeDialog.valid) nativeDialog.close()
        else _root.closeRequested()
    }

    signal openRequested()
    signal closeRequested()
    signal accepted()
    signal rejected()

    Component.onCompleted: {
        if (!nativeDialog.valid)
        {
            console.debug("Create fallback Dialog.");
            qmlFileDialogComponent.createObject(_root);
        }
    }

    FileSaveDialog {
        id: nativeDialog
        title: _root.title
        filename: _root.filename

        // fileUrlChanged is emitted before accepted
        onFileUrlChanged: _root.fileUrl = fileUrl
        onAccepted: _root.accepted()
        onRejected: _root.rejected()
    }

    Component {
        id: qmlFileDialogComponent

        Item {
            FileDialog {
                id: qmlFileDialog
                title: _root.title
                selectExisting: false

                // fileUrlChanged is emitted after accepted
                onFileUrlChanged: _root.fileUrl = fileUrl
                onAccepted: {
                    _root.fileUrl = fileUrl
                    _root.accepted()
                }
                onRejected: _root.rejected()
            }

            Connections {
                target: _root
                onOpenRequested: qmlFileDialog.open()
                onCloseRequested: qmlFileDialog.close()
            }
        }
    }
}
