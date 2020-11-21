/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
import QtQuick 2.4
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.2
import Kullo 1.0

import ".."
import "../buttons"
import "../dialogs"
import "../native"
import "../windows"
import "../js/shortcut.js" as SC

BackgroundCover {
    /* public */
    state: "hidden"
    signal avatarChanged()

    /* private */
    id: root
    property bool _doneLoading: false

    onStateChanged: {
        if (_doneLoading)
        {
            // this code must not be called when state is set initially

            if (state == "visible") {
                inputAvatar.refreshAvatar()
                reloadIntoUi()
                accountInfo.reload()
            }

            if (state == "hidden") {
                saveFromUi()
            }
        }
    }

    Connections {
        target: Inbox
        onExportFinished: {
            exportDataButton.setExporting(false)
            if (error) {
                exportDataErrorDialog.error = error
                exportDataErrorDialog.open()
            } else {
                exportDataFinishedDialog.open()
            }
        }
    }

    MessageDialog {
        id: exportDataFinishedDialog
        title: qsTr("Data export finished")
        text: qsTr("Your data has been exported successfully.")
    }

    MessageDialog {
        property string error: null

        id: exportDataErrorDialog
        title: qsTr("Data export failed")
        text: qsTr("Exporting your data failed:") + " " + error
    }

    Component.onCompleted: {
        _doneLoading = true
    }

    function reloadIntoUi() {
        inputName.text = Inbox.userSettings.name
        inputOrganization.text = Inbox.userSettings.organization
        inputFooter.text = Inbox.userSettings.footer
    }

    function saveFromUi() {
        Inbox.userSettings.name = inputName.text.trim()
        Inbox.userSettings.organization = inputOrganization.text.trim()
        Inbox.userSettings.footer = inputFooter.text.trim()
        Inbox.userSettings.storeTmpAvatar()
        avatarChanged()
    }

    Rectangle {
        anchors {
            horizontalCenter: parent.horizontalCenter
            verticalCenter: parent.verticalCenter
        }
        width: 650 * Hdpi.FontScalingFactor
        height: content.anchors.topMargin
                + content.implicitHeight
                + content.anchors.bottomMargin
        radius: 10 * Hdpi.FontScalingFactor

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.AllButtons
            onClicked: {} // do nothing. Prevents overlay from closing
        }

        IconButton {
            anchors {
                top: parent.top
                right: parent.right
                topMargin: 7 * Hdpi.FontScalingFactor
                rightMargin: 10 * Hdpi.FontScalingFactor
            }
            tooltip: qsTr("Close (Esc)")
            source: "/resources/scalable/close2_b.svg"
            onClicked: fadeOut()
        }

        Item {
            id: content
            anchors {
                fill: parent
                margins: 15 * Hdpi.FontScalingFactor
            }

            implicitHeight: headline.height
                            + subheadline.anchors.topMargin
                            + subheadline.height
                            + meBlock.anchors.topMargin
                            + meBlock.height
                            + footerRow.anchors.topMargin
                            + footerRow.implicitHeight

            NativeText {
                id: headline
                property string _prefix: inputName.text.trim() != ""
                                        ? inputName.text.trim()
                                        : qsTr("You")
                text: qsTr("%1 on Kullo").arg(_prefix)
                anchors {
                    top: parent.top
                    left: parent.left
                    right: parent.right
                }
                horizontalAlignment: Text.AlignLeft
                font.pointSize: Style.fontSize.shortcutOverlayHeadline
                color: Style.profileHeadline
            }

            NativeSelectableText {
                id: subheadline
                anchors {
                    top: headline.bottom
                    topMargin: 3 * Hdpi.FontScalingFactor
                    left: parent.left
                    right: parent.right
                }
                text: Inbox.userSettings.address
                horizontalAlignment: Text.AlignLeft
                color: Style.profileHeadlineSecondary

                CutCopyPasteMenu {
                    hasCopy: true
                    hasCut: false
                    hasPaste: false
                    hasSelectAll: true
                    onCopy: subheadline.copy()
                    onSelectAll: subheadline.selectAll()
                }
            }

            Item {
                id: meBlock
                anchors {
                    left: parent.left
                    right: parent.right
                    top: subheadline.bottom
                    topMargin: 20 * Hdpi.FontScalingFactor
                }
                height: Math.max(leftColumn.implicitHeight,
                                 rightColumn.implicitHeight)

                Column {
                    id: leftColumn
                    spacing: 7 * Hdpi.FontScalingFactor
                    anchors {
                        top: parent.top
                        left: parent.left
                    }
                    width: inputAvatar.width

                    AvatarChooser {
                        id: inputAvatar
                        address: Inbox.userSettings.address
                        onAvatarChanged: root.avatarChanged()
                    }
                }

                Column {
                    id: rightColumn
                    spacing: 7 * Hdpi.FontScalingFactor
                    anchors {
                        top: parent.top
                        left: leftColumn.right
                        leftMargin: 20 * Hdpi.FontScalingFactor
                        right: parent.right
                    }

                    Column {
                        spacing: 1 * Hdpi.FontScalingFactor
                        id: nameGroup

                        anchors {
                            left: parent.left
                            right: parent.right
                        }

                        TextField {
                            id: inputName
                            placeholderText: qsTr("Name")
                            anchors {
                                left: parent.left
                                right: parent.right
                            }
                        }

                        NativeText {
                            visible: inputName.text.trim() === ""
                            text: qsTr("Name must be set in order to send messages")
                            color: Style.red
                            anchors {
                                left: parent.left
                                right: parent.right
                            }
                        }
                    }

                    TextField {
                        id: inputOrganization
                        placeholderText: qsTr("Organization")
                        anchors {
                            left: parent.left
                            right: parent.right
                        }
                    }

                    TextArea {
                        id: inputFooter
                        //placeholderText: qsTr("Footer")
                        height: 202 - 2*rightColumn.spacing - nameGroup.height - inputOrganization.height
                        anchors {
                            left: parent.left
                            right: parent.right
                        }
                    }
                }
            }

            Row {
                id: footerRow

                anchors {
                    top: meBlock.bottom
                    topMargin: 20 * Hdpi.FontScalingFactor
                    left: parent.left
                    right: parent.right
                }

                spacing: 10 * Hdpi.FontScalingFactor
                property real buttonHeight: 40 * Hdpi.FontScalingFactor

                AccountInfo {
                    id: accountInfo
                    inbox: Inbox
                }

                Button {
                    height: footerRow.buttonHeight
                    text: qsTr("Show MasterKey")
                    style: KulloButtonStyle {}

                    onClicked: {
                        masterKeyShowWindow.openWindow()
                        masterKeyShowWindow.raise()
                        masterKeyShowWindow.requestActivate()
                    }

                    MasterKeyShowWindow {
                        id: masterKeyShowWindow
                        address: Inbox.userSettings.address
                        key: Inbox.userSettings.masterKeyPem
                    }
                }

                Button {
                    height: footerRow.buttonHeight
                    text: qsTr("Account settings")
                    style: KulloButtonStyle {}
                    onClicked: accountInfo.openSettingsLocationUrl()
                }

                Button {
                    id: exportDataButton

                    height: footerRow.buttonHeight
                    text: qsTr("Export your data")
                    style: KulloButtonStyle {}
                    onClicked: {
                        setExporting(true)
                        exportDataDialog.openDialog()
                    }

                    function setExporting(isExporting) {
                        enabled = !isExporting
                        if (isExporting) {
                            text = qsTr("Exporting…")
                        } else {
                            text = qsTr("Export your data")
                        }
                    }
                }

                AdvancedFileSaveDialog {
                    id: exportDataDialog

                    title: qsTr("Export your data")
                    filename: "KulloExport"

                    onAccepted: {
                        Inbox.exportDataAsync(fileUrl)
                    }

                    onRejected: {
                        exportDataButton.setExporting(false)
                    }
                }
            }
        }
    }
}
