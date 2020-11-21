/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
.pragma library

function scheme_from_url(urlString) {
    var schemeEndPos = urlString.indexOf(":")
    if (schemeEndPos === -1) return null

    var scheme = urlString.substring(0, schemeEndPos)
    return scheme
}

function humanDatetime(datetime, dateFormat, timeFormat) {
    var now = new Date()
    var todayAtMidnight = new Date(now.getFullYear(), now.getMonth(), now.getDate())
    var yesterdayAtMidnight = new Date(now.getFullYear(), now.getMonth(), now.getDate() -1)

    if (datetime.getTime() > todayAtMidnight.getTime())
    {
        return datetime.toLocaleTimeString(Qt.locale(), timeFormat)
    }
    else if (datetime.getTime() > yesterdayAtMidnight.getTime())
    {
        return qsTr("yesterday") + ", "
                + datetime.toLocaleTimeString(Qt.locale(), timeFormat)
    }
    else
    {
        return datetime.toLocaleDateString(Qt.locale(), dateFormat)
    }
}

function humanFullDatetime(datetime, dateFormat, timeFormat) {
    return datetime.toLocaleDateString(Qt.locale(), dateFormat)
            + " "
            + datetime.toLocaleTimeString(Qt.locale(), timeFormat)
}

function filesize_human(bytes) {
    // Use binary multiplier 1024 here to avoid having a "102 MB" file in Kullo
    // indicating that files greater that 100 megabytes can be sent.
    // Use unit titles "MB" and "KB" known from Windows Explorer.
    var MEGA = 1024*1024
    var KILO = 1024

    if (bytes >= 80*MEGA)
        return (bytes/MEGA).toLocaleString(Qt.locale(), 'f', 0) + " MB"
    else if (bytes >= 1*MEGA)
        return (bytes/MEGA).toLocaleString(Qt.locale(), 'f', 1) + " MB"
    else if (bytes >= 8*KILO)
        return (bytes/KILO).toLocaleString(Qt.locale(), 'f', 0) + " KB"
    else if (bytes >= 1*KILO)
        return (bytes/KILO).toLocaleString(Qt.locale(), 'f', 1) + " KB"
    else
        return bytes + " Bytes"
}

function pad_left(input, size, padding_char) {
    padding_char = typeof padding_char !== 'undefined' ? padding_char : '0';
    var s = input + "";
    while (s.length < size) s = padding_char + s;
    return s;
}
