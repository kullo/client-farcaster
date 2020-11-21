/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
.pragma library

// Returns nothing. This is in-place
function removeFromArray(array, what)
{
    var pos;
    while ((pos = array.indexOf(what)) !== -1)
    {
        array.splice(pos, 1);
    }
}

function sizeOfMapObject(obj) {
    return Object.keys(obj).length;
}
