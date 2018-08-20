/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
import QtQuick 2.4

AttachmentDelegate {
    selected: attachmentsList.currentIndex == index
    listHasActiveFocus: attachmentsList.activeFocus

    attachmentId: index

    size: size_
    filename: filename_
    hash: hash_
    mimeType: mimeType_
}
