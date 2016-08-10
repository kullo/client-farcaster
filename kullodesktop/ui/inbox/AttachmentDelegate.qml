/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
import QtQuick 2.4

import ".."
import "../dialogs"
import "../misc"
import "../native"
import "../js/format.js" as Format
import "../js/mime.js" as Mime

Item {
    /* public */
    property int paddingTop: 3
    property int paddingBottom: 3
    property int paddingLeft: 3
    property int paddingRight: 3

    property bool selected: false
    property bool listHasActiveFocus: false

    property int size
    property string filename
    property string hash
    property string mimeType
    property int conversationId: -1
    property int messageId: -1
    property int attachmentId: -1
    property bool attachmentsReady: false

    implicitHeight: paddingTop
                    + bodyContent.implicitHeight
                    + paddingBottom

    id: root

    SystemPalette {
        id: palette
        colorGroup: root.enabled ? SystemPalette.Active : SystemPalette.Disabled
    }

    Rectangle {
        id: body
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        radius: 3
        color: selected ?
                   listHasActiveFocus ? palette.highlight : palette.alternateBase : "transparent"

        TooltipArea {
            text: filename

            Column {
                id: bodyContent

                anchors {
                    fill: parent
                    topMargin: paddingTop
                    leftMargin: paddingLeft
                    rightMargin: paddingRight
                }
                spacing: 2

                NativeImage {
                    id: fileIcon

                    property int _WIDTH: 120
                    property int _HEIGHT: 48
                    function _isImage(mime)
                    {
                        return mime === "image/bmp"
                                || mime === "image/gif"
                                || mime === "image/jpeg"
                                || mime === "image/png"
                    }

                    source: attachmentsReady && _isImage(mimeType)
                            ? "image://attachmentpreview/" + Utils.urlencode(Client.userSettings.address)
                              + "/" + conversationId
                              + "/" + messageId
                              + "/" + attachmentId
                              + "?" + hash
                            : Utils.makeFileUrl("/icons/filetypes/ubuntu/" + Mime.mime2icon(mimeType))
                    anchors {
                        horizontalCenter: parent.horizontalCenter
                    }

                    width: _WIDTH
                    height: _HEIGHT
                    fillMode: Image.PreserveAspectFit

                    //onPaintedGeometryChanged: {
                    //    console.warn(paintedWidth + "x" + paintedHeight)
                    //}

                    asynchronous: true
                }

                NativeText {
                    id: textFilename
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                    text: filename
                    font.pointSize: Style.fontSize.attachmentFilename
                    horizontalAlignment: Text.AlignHCenter
                    elide: Text.ElideMiddle
                    wrapMode: Text.NoWrap
                    maximumLineCount: 1

                    color: (selected && listHasActiveFocus) ?
                               palette.highlightedText : palette.text
                }

                NativeText {
                    id: textFilesize
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                    text: Format.filesize_human(size)
                    font.pointSize: Style.fontSize.attachmentFilesize
                    horizontalAlignment: Text.AlignHCenter
                    color: (selected && listHasActiveFocus) ?
                               palette.midlight : palette.mid
                }
            }
        }
    }
}
