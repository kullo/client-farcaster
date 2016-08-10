/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
.pragma library

function filesize_human(bytes) {
    // Use binary multiplyer 1024 here to avoid having a "102 MB" file in Kullo
    // indicating that files greater that 100 megabytes can be sent.
    // Use unit titles "MB" and "KB" known from Windows Explorer.
    var MEGA = 1024*1024
    var KILO = 1024

    if (bytes >= 80*MEGA)
        return (bytes/MEGA).toFixed(0) + " MB"
    else if (bytes >= 1*MEGA)
        return (bytes/MEGA).toFixed(1) + " MB"
    else if (bytes >= 8*KILO)
        return (bytes/KILO).toFixed(0) + " KB"
    else if (bytes >= 1*KILO)
        return (bytes/KILO).toFixed(1) + " KB"
    else
        return bytes + "  Bytes"
}

function pad_left(input, size, padding_char) {
    padding_char = typeof padding_char !== 'undefined' ? padding_char : '0';
    var s = input + "";
    while (s.length < size) s = padding_char + s;
    return s;
}
