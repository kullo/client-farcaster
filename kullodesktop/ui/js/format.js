/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
.pragma library

function scheme_from_url(urlString) {
    var schemeEndPos = urlString.indexOf(":")
    if (schemeEndPos === -1) return null

    var scheme = urlString.substring(0, schemeEndPos)
    return scheme
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
