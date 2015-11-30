/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3

import "../"
import "../js/djb2.js" as DJB2
import "../buttons"
import "../dialogs"
import "../misc"

Item {
    id: root

    property alias name: label.text
    property string address: "";

    property int marginTop: 0
    property int marginBottom: 0
    property int labelOffset: 0

    function refreshAvatar() {
        avatar.source = (address !== "")
                ? "image://usersettingsavatars/" + address + "?" + Date.now()
                : ""
    }

    signal avatarChanged()

    anchors {
        left: parent.left
        right: parent.right
    }

    height: marginTop + input.height + marginBottom

    RowLabel {
        id: label
        y: marginTop + labelOffset
    }

    Rectangle {
        id: input
        color: Style.black
        width: imageContent.width + 2*imageContent.x
        height: width

        anchors {
            left: label.right
        }
        y: marginTop

        ChessRectangle {
            id: imageContent
            x: 1 // border
            y: x
            width: 180
            height: width

            Image {
                id: avatar
                width: parent.width
                height: width

                sourceSize.width: width
                sourceSize.height: height
                smooth: true

                cache: false

                AdvancedFileOpenDialog {
                    id: selectAvatarDialog
                    title: qsTr("Select avatar")
                    onAccepted: {
                        console.debug("New avatar: " + fileUrl)
                        if (Client.userSettings.setTmpAvatarFileUrl(fileUrl))
                        {
                            console.debug("Temporary avatar set.")
                            refreshAvatar()
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
                                && Client.userSettings.avatarMimeType !== ""
                    }

                    DropArea {
                        anchors.fill: parent

                        onDropped: {
                            if (drop.hasUrls && drop.action == Qt.CopyAction)
                            {
                                var fileUrl = drop.urls[0];
                                console.debug("New avatar: " + fileUrl)
                                if (Client.userSettings.setTmpAvatarFileUrl(fileUrl))
                                {
                                    console.debug("Temporary avatar set.")
                                    Client.userSettings.tmpAvatarActive = true
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
                            topMargin: 4
                            rightMargin: 4
                        }
                        visible: false

                        onClicked: {
                            // deleteConfirmDialog.openDialog()
                            //
                            // This deletes avatar without further question
                            // because only one modal window is possible
                            // Fix as soon as delete confirmation for messages is there
                            Client.userSettings.deleteAvatar()
                            refreshAvatar()
                            avatarChanged()
                        }

                        tooltip: qsTr("Delete avatar")
                        backgroundColor: "#aaffffff"
                        hoverColor: "#20ffffff"
                        source: "/resources/scalable/delete_b.svg"

//                      YesNoDialog {
//                          id: deleteConfirmDialog
//                          objectName: "deleteAvatarConfirmation"
//                          title: qsTr("Confirmation")
//                          text: qsTr("Do you really want to delete your avatar?")
//                          onYes: {
//                              console.info("User wants to delete avatar.")
//                              Client.userSettings.deleteAvatar()
//                              refreshAvatar()
//                          }
//                      }
                    }
                }
            }
        }
    }
}
