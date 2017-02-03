import QtQuick 2.0

NativeText {
    /* public */
    property string html: ""
    property color linkColor: root.color

    function makeText() {
        var cssColor = root.linkColor
        var modifiedHtml = html.replace(
                    "<a ",
                    "<a style='color: " + cssColor + "' "
                    )
        root.text = modifiedHtml
    }

    onHtmlChanged: makeText()
    onLinkColorChanged: makeText()

    id: root
    text: html
    textFormat: Text.RichText
    onLinkActivated: Qt.openUrlExternally(link)
}
