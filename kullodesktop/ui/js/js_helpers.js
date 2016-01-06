/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
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
