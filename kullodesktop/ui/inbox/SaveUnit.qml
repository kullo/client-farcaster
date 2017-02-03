/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import Kullo 1.0

StableTimer {
    /* public */
    property int maxCount: 50
    property int maxTime: 15 // seconds
    signal save()
    function count() { _counter++ }

    /* private */
    property int _counter: 0

    id: root
    interval: maxTime*1000
    triggeredOnStart: false
    repeat: true
    running: true

    onTriggered: {
        if (_counter > 0) {
            console.debug("Save draft from timer. (changes count: " + _counter + ")")
            root.save()
            _counter = 0
            root.restart()
        }
    }

    on_CounterChanged: {
        if (_counter >= maxCount) {
            console.debug("Save draft from counter. (changes count: " + _counter + ")")
            root.save()
            _counter = 0
            root.restart()
        }
    }
}
