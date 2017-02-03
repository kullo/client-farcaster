/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Controls 1.3
import Kullo 1.0

import "." // QTBUG-34418, singletons require explicit import to load qmldir file
import "buttons"
import "dialogs"
import "formelements"
import "misc"
import "native"

FocusScope {
    anchors.fill: parent

    function reset() {
        if (mainloader.item) mainloader.item.reset()
    }

    Component.onCompleted: {
        reset()
        step1()
        registerer.generateKeys()
    }

    Registerer {
        id: registerer
        application: InnerApplication
        client: Inbox.client()
    }

    function onLoadedStep1() {
        mainloader.item.registerer = registerer
        mainloader.item.done.connect(onStep1Done)
        mainloader.item.forceActiveFocus()
    }

    function onLoadedStep2() {
        mainloader.item.registerer = registerer
        mainloader.item.done.connect(onStep2Done)
        mainloader.item.forceActiveFocus()
    }

    function onStep1Done() {
        appWindow.alert(0)
        step2()
    }

    function onStep2Done() {
        app.state = "loggingIn"
    }

    function step1() {
        // diconnect all from loader
        mainloader.loaded.disconnect(onLoadedStep1);
        mainloader.loaded.disconnect(onLoadedStep2);

        mainloader.loaded.connect(onLoadedStep1);
        mainloader.source = "/registration/Keygen.qml"
    }

    function step2() {
        // diconnect all from loader
        mainloader.loaded.disconnect(onLoadedStep1);
        mainloader.loaded.disconnect(onLoadedStep2);

        mainloader.loaded.connect(onLoadedStep2);
        mainloader.source = "/registration/AddressSelection.qml"
    }

    Rectangle {
        anchors.fill: parent
        color: Style.beigeLight

        Logo {
            id: logo
            anchors {
                top: parent.top
                right: parent.right
                topMargin: 10
                rightMargin: anchors.topMargin
            }
        }

        Loader {
            id: mainloader
            focus: true

            anchors {
                top: logo.bottom
                topMargin: 50
                left: parent.left
                right: parent.right
                bottom: parent.bottom
                leftMargin: 20
                rightMargin: 20
            }
        }
    }
}
