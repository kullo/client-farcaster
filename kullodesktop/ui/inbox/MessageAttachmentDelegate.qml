/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
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
