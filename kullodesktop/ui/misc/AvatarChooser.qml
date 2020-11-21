/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
import QtQuick 2.0

import "../"
import "../buttons"
import "../dialogs"
import "../native"

Rectangle {
    /* public */
    property string address: ""
    signal avatarChanged()

    /* private */
    property int _borderWidth: 1 * Hdpi.FontScalingFactor
    id: root
    color: Style.black

    implicitHeight: 200 * Hdpi.FontScalingFactor + 2 * _borderWidth
    implicitWidth: 200 * Hdpi.FontScalingFactor + 2 * _borderWidth
    width: 200 * Hdpi.FontScalingFactor + 2 * _borderWidth
    height: 200 * Hdpi.FontScalingFactor + 2 * _borderWidth

    function refreshAvatar() {
        avatar.source = (address !== "")
                ? "image://usersettingsavatars/" + address + "?" + Date.now()
                : ""
    }

    ChessRectangle {
        id: imageContent
        x: _borderWidth // border
        y: _borderWidth
        width: root.width-2*_borderWidth
        height: root.height-2*_borderWidth

        NativeImage {
            id: avatar
            width: parent.width
            height: parent.height

            smooth: true
            asynchronous: true
            cache: false

            AdvancedFileOpenDialog {
                id: selectAvatarDialog
                title: qsTr("Select avatar")
                onAccepted: {
                    console.debug("New avatar: " + fileUrl)
                    if (Inbox.userSettings.setTmpAvatarFileUrl(fileUrl))
                    {
                        console.debug("Temporary avatar set.")
                        refreshAvatar()
                        avatarChanged()
                    }
                }
            }

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    selectAvatarDialog.openDialog();
                }
                onHoveredChanged: {
                    deleteButton.visible = containsMouse
                            && Inbox.userSettings.avatarMimeType !== ""
                }

                DropArea {
                    anchors.fill: parent

                    onDropped: {
                        if (drop.hasUrls && drop.action == Qt.CopyAction)
                        {
                            var fileUrl = drop.urls[0];
                            console.debug("New avatar: " + fileUrl)
                            if (Inbox.userSettings.setTmpAvatarFileUrl(fileUrl))
                            {
                                console.debug("Temporary avatar set.")
                                refreshAvatar()
                            }
                            drop.acceptProposedAction()
                        }
                        else {
                            drop.accepted = false
                        }
                    }
                }

                IconButton {
                    id: deleteButton
                    anchors {
                        top: parent.top
                        right: parent.right
                        topMargin: 4 * Hdpi.FontScalingFactor
                        rightMargin: 4 * Hdpi.FontScalingFactor
                    }
                    visible: false

                    onClicked: deleteConfirmDialog.openDialog()

                    tooltip: qsTr("Delete avatar")
                    backgroundColor: "#aaffffff"
                    hoverColor: "#20ffffff"
                    source: "/resources/scalable/delete_b.svg"

                    YesNoDialog {
                        id: deleteConfirmDialog
                        objectName: "deleteAvatarConfirmation"
                        title: qsTr("Confirmation")
                        text: qsTr("Do you really want to delete your avatar?")
                        onYes: {
                            console.info("User wants to delete avatar.")
                            Inbox.userSettings.deleteAvatar()
                            refreshAvatar()
                            avatarChanged()
                        }
                    }
                }
            }
        }
    }
}
