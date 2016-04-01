/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import Kullo 1.0

import "." // QTBUG-34418, singletons require explicit import to load qmldir file
import "native"

Item {
    id: root
    anchors.fill: parent

    Component.onCompleted: {
        timer.start()
        Client.prepareLogin()
    }

    Connections {
        target: Client
        onLoginPrepared: {
            Client.logIn()
        }
        onLoggedInChanged: {
            timer.stop()
            if (loggedIn)
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

    Timer {
        id: timer
        interval: 250
        repeat: false
        onTriggered: {
            content.createObject(root);
        }
    }

    Component {
        id: content

        NativeText {
            id: text
            anchors {
                verticalCenter: parent.verticalCenter
                left: parent.left
            }
            Component.onCompleted: {
                anchors.leftMargin = (root.width - text.implicitWidth)/2
                dotTimer.start()
            }
            color: Style.gray
            font.pointSize: Style.fontSize.loggingIn

            property int dotCount: 3
            text: qsTr("Logging in") + " " + "...".slice(0, dotCount);

            Timer {
                id: dotTimer
                interval: 800
                repeat: true
                triggeredOnStart: false
                onTriggered: text.dotCount = (text.dotCount+1)%4;
            }
        }
    }
}
