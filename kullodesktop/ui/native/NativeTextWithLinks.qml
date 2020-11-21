/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
import QtQuick 2.0

NativeText {
    textFormat: Text.StyledText

    // Only use this component for default links like http:// or file://
    // because we cannot override slots like onLinkActivated
    onLinkActivated: {
        if (!Qt.openUrlExternally(link))
        {
            console.warn("Could not open url: " + link)
        }
    }
}
