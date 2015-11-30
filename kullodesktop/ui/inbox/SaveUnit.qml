/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
import QtQuick 2.4

Timer {
    // public
    property int maxCount: 50
    property int maxTime: 15 // seconds
    signal save()
    function count() { pCounter++ }


    // private stuff
    property int pCounter: 0

    id: saveTimer
    interval: maxTime*1000
    triggeredOnStart: false
    repeat: true
    running: true

    onTriggered: {
        if (pCounter > 0) {
            console.debug("Save draft from timer. (changes count: " + pCounter + ")")
            saveTimer.save()
            pCounter = 0
            saveTimer.restart()
        }
    }

    onPCounterChanged: {
        if (pCounter >= maxCount) {
            console.debug("Save draft from counter. (changes count: " + pCounter + ")")
            saveTimer.save()
            pCounter = 0
            saveTimer.restart()
        }
    }
}
