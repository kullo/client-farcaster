/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Dialogs 1.2
import Kullo 1.0

Item {
    id: _root
    property string title
    property url fileUrl
    property var fileUrls
    property bool selectMultiple: false

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

    FileOpenDialog {
        id: nativeDialog
        title: _root.title
        selectMultiple: _root.selectMultiple

        // fileUrlChanged is emitted before accepted
        onFileUrlChanged: _root.fileUrl = fileUrl
        onFileUrlsChanged: _root.fileUrls = fileUrls
        onAccepted: _root.accepted()
        onRejected: _root.rejected()
    }

    Component {
        id: qmlFileDialogComponent

        Item {
            FileDialog {
                id: qmlFileDialog
                title: _root.title
                selectExisting: true
                selectMultiple: _root.selectMultiple

                // fileUrlChanged is emitted after accepted
                onFileUrlChanged: _root.fileUrl = fileUrl
                onFileUrlsChanged: _root.fileUrls = fileUrls
                onAccepted: {
                    _root.fileUrl = fileUrl
                    _root.fileUrls = fileUrls
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
