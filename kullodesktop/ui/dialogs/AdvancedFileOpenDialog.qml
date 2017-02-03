/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Dialogs 1.2
import Kullo 1.0

Item {
    id: root
    property string title
    property url fileUrl
    property var fileUrls
    property bool selectMultiple: false

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

    FileOpenDialog {
        id: nativeDialog
        title: root.title
        selectMultiple: root.selectMultiple

        // fileUrlChanged is emitted before accepted
        onFileUrlChanged: root.fileUrl = fileUrl
        onFileUrlsChanged: root.fileUrls = fileUrls
        onAccepted: root.accepted()
        onRejected: root.rejected()
    }

    Component {
        id: qmlFileDialogComponent

        Item {
            FileDialog {
                id: qmlFileDialog
                title: root.title
                selectExisting: true
                selectMultiple: root.selectMultiple

                // fileUrlChanged is emitted after accepted
                onFileUrlChanged: root.fileUrl = fileUrl
                onFileUrlsChanged: root.fileUrls = fileUrls
                onAccepted: {
                    root.fileUrl = fileUrl
                    root.fileUrls = fileUrls
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
