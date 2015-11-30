/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Controls 1.3

import ".."
import "../native"
import "../js/format.js" as Format
import "../js/mime.js" as Mime

Item {
    property int paddingTop: 3
    property int paddingLeft: 3
    property int paddingRight: 3

    id: _root

    SystemPalette {
        id: palette
        colorGroup: _root.enabled ? SystemPalette.Active : SystemPalette.Disabled
    }

    Rectangle {
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
            anchors {
                fill: parent
                topMargin: paddingTop
                leftMargin: paddingLeft
                rightMargin: paddingRight
            }
            spacing: 2

            Image {
                id: _fileIcon

                function isImage(mime)
                {
                    return mime === "image/bmp"
                            || mime === "image/gif"
                            || mime === "image/jpeg"
                            || mime === "image/png"
                }

                source: isImage(mimeType_)
                        ? "image://attachmentpreview/" + Utils.urlencode(Client.userSettings.address)
                          + "/" + conversationId + "/0/" + attachmentIndex_
                          + "?" + hash_
                        : Utils.makeFileUrl("/icons/filetypes/ubuntu/" + Mime.mime2icon(mimeType_))
                width: parent.width
                height: 48
                smooth: true
                sourceSize.height: 48
                sourceSize.width: 48
                fillMode: Image.PreserveAspectFit
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
