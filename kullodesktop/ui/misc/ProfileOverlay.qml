/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
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
        width: 650
        height: content.anchors.topMargin
                + content.implicitHeight
                + content.anchors.bottomMargin
        radius: 10

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.AllButtons
            onClicked: {} // do nothing. Prevents overlay from closing
        }

        IconButton {
            anchors {
                top: parent.top
                right: parent.right
                topMargin: 7
                rightMargin: 10
            }
            tooltip: qsTr("Close (Esc)")
            source: "/resources/scalable/close2_b.svg"
            onClicked: fadeOut()
        }

        Item {
            id: content
            anchors {
                fill: parent
                topMargin: 15
                leftMargin: 15
                rightMargin: 15
                bottomMargin: 15
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
                    topMargin: 3
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
                    topMargin: 20
                }
                height: Math.max(leftColumn.implicitHeight,
                                 rightColumn.implicitHeight)

                Column {
                    id: leftColumn
                    spacing: 7
                    width: 200
                    anchors {
                        top: parent.top
                        left: parent.left
                    }

                    AvatarChooser {
                        id: inputAvatar
                        address: Inbox.userSettings.address
                        onAvatarChanged: root.avatarChanged()
                    }
                }

                Column {
                    id: rightColumn
                    spacing: 7
                    anchors {
                        top: parent.top
                        left: leftColumn.right
                        leftMargin: 20
                        right: parent.right
                    }

                    Column {
                        spacing: 1
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
                    topMargin: 20
                    left: parent.left
                    right: parent.right
                }

                spacing: 10

                AccountInfo {
                    id: accountInfo
                    inbox: Inbox
                }

                Button {
                    height: 40
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
                    height: 40
                    text: qsTr("Account settings")
                    style: KulloButtonStyle {}
                    onClicked: accountInfo.openSettingsLocationUrl()
                }

                Button {
                    height: 40
                    text: qsTr("Plan: %1 (%2/%3 GB)")
                    .arg(accountInfo.planName)
                    .arg((accountInfo.storageUsed/(1024*1024*1024)).toLocaleString(Qt.locale(), 'f', 2))
                    .arg(accountInfo.storageQuota/(1024*1024*1024))
                    style: KulloButtonStyle {
                        progress: accountInfo.storageUsed
                        progressMax: accountInfo.storageQuota
                        progressBarColor: backgroundColor
                        progressBarBackgroundColor: Style.white
                        progressBarBorderColor: Utils.setAlpha(Style.white, 0.33)
                    }

                    onClicked: accountInfo.openSettingsLocationUrl()
                }
            }
        }
    }
}
