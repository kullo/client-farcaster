/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
.pragma library

function filesize_human(bytes) {
    if (bytes > 8000000)
        return Math.round(bytes/1000000, 0) + " MB";
    else if (bytes > 1000000)
        return Math.round(bytes/1000000, 1) + " MB";
    else if (bytes > 8000)
        return Math.round(bytes/1000, 0) + " kB";
    else if (bytes > 1000)
        return Math.round(bytes/1000, 1) + " kB";
    else
        return bytes + "  Bytes";
}

function pad_left(input, size, padding_char) {
    padding_char = typeof padding_char !== 'undefined' ? padding_char : '0';
    var s = input + "";
    while (s.length < size) s = padding_char + s;
    return s;
}
