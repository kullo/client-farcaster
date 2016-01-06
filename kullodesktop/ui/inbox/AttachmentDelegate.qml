/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
import QtQuick 2.4

import ".."
import "../dialogs"
import "../native"
import "../js/format.js" as Format
import "../js/mime.js" as Mime

Item {
    property int paddingTop: 3
    property int paddingBottom: 3
    property int paddingLeft: 3
    property int paddingRight: 3
    property bool parentHasActiveFocus: false

    property int conversationId: -1
    property int messageId: -1
    property bool attachmentsReady: false

    implicitHeight: paddingTop
                    +_bodyContent.implicitHeight
                    + paddingBottom

    id: _root

    SystemPalette {
        id: palette
        colorGroup: _root.enabled ? SystemPalette.Active : SystemPalette.Disabled
    }

    Rectangle {
        id: _body
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        radius: 3
        color: attachmentsList.currentIndex == index ?
                   attachmentsList.activeFocus ? palette.highlight : palette.alternateBase : "transparent"

        Column {
            id: _bodyContent

            anchors {
                fill: parent
                topMargin: paddingTop
                leftMargin: paddingLeft
                rightMargin: paddingRight
            }
            spacing: 2

            Image {
                id: _fileIcon
                source: attachmentsReady && (mimeType_ === "image/bmp"
                                             || mimeType_ === "image/gif"
                                             || mimeType_ === "image/jpeg"
                                             || mimeType_ === "image/png")
                        ? "image://attachmentpreview/" + Utils.urlencode(Client.userSettings.address) + "/" + conversationId + "/" + messageId + "/" + index
                        : Utils.makeFileUrl("/icons/filetypes/ubuntu/" + Mime.mime2icon(mimeType_))
                anchors {
                    left: parent.left
                    leftMargin: 10
                }
                width: parent.width-2*anchors.leftMargin
                height: 48
                smooth: true
                sourceSize.height: 48
                sourceSize.width: 48
                fillMode: Image.PreserveAspectFit
                asynchronous: true
            }

            NativeText {
                id: textFilename
                anchors {
                    left: parent.left
                    right: parent.right
                }
                text: filename_
                font.pointSize: Style.fontSize.attachmentFilename
                horizontalAlignment: Text.AlignHCenter
                elide: Text.ElideMiddle
                wrapMode: Text.NoWrap
                maximumLineCount: 1

                color: (attachmentsList.currentIndex == index  && attachmentsList.activeFocus) ?
                           palette.highlightedText : palette.text
            }

            NativeText {
                id: textFilesize
                anchors {
                    left: parent.left
                    right: parent.right
                }
                text: Format.filesize_human(size_)
                font.pointSize: Style.fontSize.attachmentFilesize
                horizontalAlignment: Text.AlignHCenter
                color: (attachmentsList.currentIndex == index  && attachmentsList.activeFocus) ?
                           palette.midlight : palette.mid
            }
        }
    }
}
