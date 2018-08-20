/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3

import "../misc"

Item {
    /* public */
    property alias name: label.text
    property alias address: input.address

    property int marginTop: 0
    property int marginBottom: 0
    property int labelOffset: 0

    function refreshAvatar() {
        input.refreshAvatar()
    }

    signal avatarChanged()

    /* private */
    id: root
    anchors {
        left: parent.left
        right: parent.right
    }

    height: marginTop + input.height + marginBottom

    RowLabel {
        id: label
        y: marginTop + labelOffset
    }

    AvatarChooser {
        id: input
        anchors {
            left: label.right
        }
        y: marginTop
        onAvatarChanged: root.avatarChanged()
    }
}
