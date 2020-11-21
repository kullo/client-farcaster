/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
import QtQuick 2.4
import QtQuick.Dialogs 1.2
import Kullo 1.0

Item {
    id: root
    property string title
    property string filename
    property url fileUrl

    function openDialog() {
        if (nativeDialog.valid) nativeDialog.open()
        else root.openRequested()
    }

    function closeDialog() {
        if (nativeDialog.valid) nativeDialog.close()
        else root.closeRequested()
    }

    signal openRequested()
    signal closeRequested()
    signal accepted()
    signal rejected()

    Component.onCompleted: {
        if (!nativeDialog.valid)
        {
            console.debug("Create fallback Dialog.");
            qmlFileDialogComponent.createObject(root);
        }
    }

    FileSaveDialog {
        id: nativeDialog
        title: root.title
        filename: root.filename

        // fileUrlChanged is emitted before accepted
        onFileUrlChanged: root.fileUrl = fileUrl
        onAccepted: root.accepted()
        onRejected: root.rejected()
    }

    Component {
        id: qmlFileDialogComponent

        Item {
            FileDialog {
                id: qmlFileDialog
                title: root.title
                selectExisting: false

                // fileUrlChanged is emitted after accepted
                onFileUrlChanged: root.fileUrl = fileUrl
                onAccepted: {
                    root.fileUrl = fileUrl
                    root.accepted()
                }
                onRejected: root.rejected()
            }

            Connections {
                target: root
                onOpenRequested: qmlFileDialog.open()
                onCloseRequested: qmlFileDialog.close()
            }
        }
    }
}
