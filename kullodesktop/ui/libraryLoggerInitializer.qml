/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
import QtQuick 2.4
import Kullo 1.0

LibraryLoggerQmlConnector {
    Component.onCompleted: {
        console.error = function(msg) {
            var stackList = (new Error).stack.split("\n")
            stackList.shift()
            e(msg, stackList.join("\n"))
        }

        console.warn = function(msg) {
            var stackList = (new Error).stack.split("\n")
            stackList.shift()
            w(msg, stackList.join("\n"))
        }

        console.info = function(msg) {
            var stackList = (new Error).stack.split("\n")
            stackList.shift()
            i(msg, stackList.join("\n"))
        }

        console.debug = function(msg) {
            var stackList = (new Error).stack.split("\n")
            stackList.shift()
            d(msg, stackList.join("\n"))
        }

        console.debug("Done initializing LibraryLogger.")
    }
}
