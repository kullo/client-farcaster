/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
import QtQuick 2.0

NativeText {
    /* public */
    property string html: ""
    property color linkColor: root.color

    property var handleOnLinkActivated: superHandleOnLinkActivated
    // "super" from the instance's perspective. Use this in implementations of handleOnLinkActivated
    property var superHandleOnLinkActivated: handleOnLinkActivated_NativeTextWithLinks

    /* private */
    function _makeText() {
        var cssColor = root.linkColor
        var modifiedHtml = html.replace(
                    "<a ",
                    "<a style='color: " + cssColor + "' "
                    )
        root.text = modifiedHtml
    }

    onHtmlChanged: _makeText()
    onLinkColorChanged: _makeText()

    id: root
    text: html
    textFormat: Text.RichText

    function handleOnLinkActivated_NativeTextWithLinks(link) {
        if (!Qt.openUrlExternally(link))
        {
            console.warn("Could not open url: " + link)
        }
    }

    // We cannot override slots
    // http://stackoverflow.com/questions/34948874/prevent-inherited-signal-handlers-from-executing
    onLinkActivated: handleOnLinkActivated(link)
}
