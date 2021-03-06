/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
import QtQuick 2.4
import Kullo 1.0

// This timer does autostart
// superHandleOnTriggered MUST be called when overriding handleOnTriggered
StableTimer {
    /* public */
    property int firstInterval
    property int secondInterval

    /* private */
    id: root
    property bool _isFirstInterval: true

    interval: firstInterval
    repeat: false
    running: true

    // http://stackoverflow.com/a/41817456/2013738

    property var handleOnTriggered: superHandleOnTriggered
    // "super" from the instance's perspective. Use this in implementations of handleOnTriggered
    property var superHandleOnTriggered: handleOnTriggered_DelayedRepeatingTimer

    function handleOnTriggered_DelayedRepeatingTimer() {
        if (root._isFirstInterval) {
            root._isFirstInterval = false

            root.stop()
            root.interval = secondInterval
            root.repeat = true
            root.start()
        }
    }

    onTriggered: handleOnTriggered()
}
