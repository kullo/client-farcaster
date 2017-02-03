/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Controls 1.3
import Kullo 1.0

import "../native"

BaseDialog {
    id: root
    objectName: "UpdateDialog"
    property alias textUpdateAvailable: updateAvailableText.text
    property alias state: mainItem.state

    title: state == "downloading" || root.state == "downloaded"
           ? qsTr("Downloading Kullo update")
           : qsTr("Kullo update available")

    width: 500
    minimumHeight: content.implicitHeight
                   + footer.implicitHeight
                   + 2*verticalPadding

    Component.onCompleted: mainItem.forceActiveFocus()

    function recalculateContentHeight()
    {
        root.height = content.implicitHeight
                + footer.implicitHeight
                + 2*verticalPadding
    }

    Connections {
        target: content
        onImplicitHeightChanged: recalculateContentHeight()
    }

    Connections {
        target: footer
        onImplicitHeightChanged: recalculateContentHeight()
    }

    onVisibleChanged: {
        if (visible)
        {
            progressBar.value = 0
            changelogTextbox.visible = false
            changelog.update()
            recalculateContentHeight()
        }
    }

    function downloadUpdate() {
        state = "downloading"
        updater.run(KulloVersionChecker.downloadUrl)
    }

    onClosing: {
        updater.cancel()
    }

    Changelog {
        id: changelog
    }

    FocusScope {
        id: mainItem

        states: ["updateAvailable", "downloading", "downloaded"]
        state: "updateAvailable"

        anchors {
            fill: parent
            topMargin: verticalPadding
            bottomMargin: verticalPadding
            leftMargin: horizontalPadding
            rightMargin: horizontalPadding
        }
        Keys.onEscapePressed: root.closeDialog()

        Item
        {
            id: content
            anchors {
                left: parent.left
                right: parent.right
            }
            implicitHeight: root.state == "downloading" || root.state == "downloaded"
                            ? contentDownload.implicitHeight
                            : contentUpdateAvailable.implicitHeight
            height: implicitHeight

            Item {
                id: contentUpdateAvailable
                visible: root.state == "updateAvailable"
                anchors {
                    top: parent.top
                    left: parent.left
                    right: parent.right
                }
                implicitHeight: introTexts.implicitHeight

                Column {
                    id: introTexts
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                    spacing: contentFooterSpacing


                    NativeText {
                        id: updateAvailableText
                        anchors {
                            left: parent.left
                        }
                    }

                    Row {
                        id: buttons
                        anchors {
                            right: parent.right
                        }
                        layoutDirection: Qt.RightToLeft
                        spacing: buttonsSpacing

                        Keys.onReturnPressed: {
                            if (buttonYes.focus) {
                                buttonYes.clicked()
                            }
                            if (buttonNo.focus) {
                                buttonNo.clicked()
                            }
                        }

                        NativeButton {
                            id: buttonNo
                            text: qsTr("Cancel")
                            onClicked: root.closeDialog()
                            isDefault: focus
                        }

                        NativeButton {
                            id: buttonYes
                            text: qsTr("Download")
                            onClicked: {
                                root.downloadUpdate()
                            }
                            focus: true
                            isDefault: focus
                        }
                    }
                }
            }

            Column {
                id: contentDownload
                visible: root.state == "downloading"
                         || root.state == "downloaded"
                anchors {
                    fill: parent
                }
                spacing: contentFooterSpacing

                NativeText {
                    text: qsTr("Downloading latest Kullo version …")
                }

                KulloUpdater {
                    id: updater
                    onDownloadProgress: progressBar.value = percentage
                    onDone: {
                        root.state = "downloaded"
                    }
                }

                ProgressBar {
                    id: progressBar
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                    minimumValue: 0
                    maximumValue: 100
                    value: 0
                }

                Row {
                    anchors {
                        right: parent.right
                    }
                    spacing: buttonsSpacing

                    Keys.onReturnPressed: {
                        if (openButton.focus) openButton.clicked()
                        if (closeButton.focus) closeButton.clicked()
                    }

                    NativeButton {
                        id: abortButton
                        visible: root.state == "downloading"
                        text: qsTr("Abort")
                        onClicked: {
                            updater.cancel()
                            root.closeDialog()
                        }
                    }

                    NativeButton {
                        id: openButton
                        text: updater.canRunInstaller
                              ? (updater.installerNeedsClosedApp
                                 ? qsTr("Quit && Install")
                                 : qsTr("Install")
                                 )
                              : qsTr("Open")
                        visible: root.state == "downloaded"
                        onClicked: {
                            updater.openTarget()
                            root.closeDialog()
                            if (updater.installerNeedsClosedApp)
                            {
                                Qt.quit()
                            }
                        }
                        focus: true
                        isDefault: focus
                    }

                    NativeButton {
                        id: closeButton
                        text: qsTr("Close")
                        visible: root.state == "downloaded"
                        onClicked: root.closeDialog()
                        isDefault: focus
                    }
                }
            }
        }

        Item {
            id: footer
            anchors {
                left: parent.left
                right: parent.right
                top: content.bottom
                bottom: parent.bottom
            }
            implicitHeight: showDetailsText.implicitHeight
                            + (changelogTextbox.visible
                               ? changelogTextbox.anchors.topMargin + changelogTextbox.implicitHeight
                               : 0)

            NativeTextWithLinks {
                id: showDetailsText
                anchors {
                    left: parent.left
                }
                html: "<a href='showUpdateDetails://' style='color: %1'>".arg(color)
                      + (changelogTextbox.visible ? qsTr("Hide details") : qsTr("Show details"))
                      + "</a>"
                onLinkActivated: {
                    changelogTextbox.visible = !changelogTextbox.visible
                    recalculateContentHeight()
                }
            }

            TextArea {
                id: changelogTextbox
                textFormat: Text.RichText
                readOnly: true
                implicitHeight: visible ? 250 : 0
                visible: false
                text: (changelog.html !== "")
                      ? changelog.html
                      : qsTr("Changelog not available.")

                anchors {
                    top: showDetailsText.bottom
                    topMargin: 5
                    left: parent.left
                    right: parent.right
                    bottom: parent.bottom
                }
            }
        }
    }
}
