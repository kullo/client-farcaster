/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
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
