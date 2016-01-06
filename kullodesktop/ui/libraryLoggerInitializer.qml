/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import Kullo 1.0

LibraryLogger {
    Component.onCompleted: {
        console.debug("Initializing LibraryLogger ...")

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
