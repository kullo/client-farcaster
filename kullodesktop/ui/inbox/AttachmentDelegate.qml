/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
import QtQuick 2.4

import ".."
import "../dialogs"
import "../misc"
import "../native"
import "../js/format.js" as Format
import "../js/mime.js" as Mime

Item {
    property int paddingTop: 3
    property int paddingBottom: 3
    property int paddingLeft: 3
    property int paddingRight: 3

    property bool selected: false
    property bool listHasActiveFocus: false

    /* model data */
    property int size
    property string filename
    property string hash
    property string mimeType
    property int conversationId: -1
    property int messageId: -1
    property int attachmentId: -1
    property bool attachmentsReady: false

    implicitHeight: paddingTop
                    + _bodyContent.implicitHeight
                    + paddingBottom

    id: _root

    SystemPalette {
        id: _palette
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
        color: selected ?
                   listHasActiveFocus ? _palette.highlight : _palette.alternateBase : "transparent"

        TooltipArea {
            text: filename

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

                    function isImage(mime)
                    {
                        return mime === "image/bmp"
                                || mime === "image/gif"
                                || mime === "image/jpeg"
                                || mime === "image/png"
                    }

                    source: attachmentsReady && isImage(mimeType)
                            ? "image://attachmentpreview/" + Utils.urlencode(Client.userSettings.address)
                              + "/" + conversationId
                              + "/" + messageId
                              + "/" + attachmentId
                              + "?" + hash
                            : Utils.makeFileUrl("/icons/filetypes/ubuntu/" + Mime.mime2icon(mimeType))
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
                    id: _textFilename
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
                               _palette.highlightedText : _palette.text
                }

                NativeText {
                    id: _textFilesize
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                    text: Format.filesize_human(size)
                    font.pointSize: Style.fontSize.attachmentFilesize
                    horizontalAlignment: Text.AlignHCenter
                    color: (selected && listHasActiveFocus) ?
                               _palette.midlight : _palette.mid
                }
            }
        }
    }
}
