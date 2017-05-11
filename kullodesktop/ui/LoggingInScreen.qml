/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import Kullo 1.0

import "." // QTBUG-34418, singletons require explicit import to load qmldir file
import "native"

Item {
    /* private */
    property bool _isMigrating: false
    property var _mainText: null

    id: root
    anchors.fill: parent

    Component.onCompleted: {
        timer.start()
        Inbox.createSession()
    }

    Connections {
        target: Inbox
        onMigrationStarted: {
            _isMigrating = true
            _mainText.layout()
        }
        onHasSessionChanged: {
            timer.stop()
            if (hasSession)
            {
                console.info("Opening inbox view ...")
                app.state = "inbox"
            }
            else
            {
                console.error("Logged out in LogginIn view. This must not happen.")
            }
        }
    }

    StableTimer {
        id: timer
        interval: 250
        repeat: false
        onTriggered: {
            _mainText = content.createObject(root);
        }
    }

    Component {
        id: content

        NativeText {
            id: text

            /* private */
            property int _dotCount: 3

            anchors {
                verticalCenter: parent.verticalCenter
                left: parent.left
            }
            Component.onCompleted: {
                layout()
                dotTimer.start()
            }

            function layout() {
                anchors.leftMargin = (root.width - text.implicitWidth)/2
            }

            color: Style.gray
            font.pointSize: Style.fontSize.loggingIn

            text: !_isMigrating
                  ? qsTr("Logging in") + " " + "...".slice(0, _dotCount)
                  : qsTr("Optimizing inbox") + " " + "...".slice(0, _dotCount);

            StableTimer {
                id: dotTimer
                interval: 800
                repeat: true
                triggeredOnStart: false
                onTriggered: text._dotCount = (text._dotCount+1)%4;
            }
        }
    }
}
