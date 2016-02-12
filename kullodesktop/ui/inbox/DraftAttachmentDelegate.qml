/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
import QtQuick 2.4

AttachmentDelegate {
    selected: attachmentsList.currentIndex == index
    listHasActiveFocus: attachmentsList.activeFocus

    messageId: 0
    attachmentsReady: true
    attachmentId: attachmentIndex_

    size: size_
    filename: filename_
    hash: hash_
    mimeType: mimeType_
}
