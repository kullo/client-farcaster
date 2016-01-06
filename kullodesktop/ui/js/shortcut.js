/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
.pragma library

function eventToString(e)
{
    var modifiersOut = _getModifiers(e)
    var keyname = _getKeyname(e)
    return "key=" + keyname + ", modifiers=" + modifiersOut.join("+") + ", accepted=" + e.accepted
}

function _getKeyname(e)
{
    // Preserve order from http://doc.qt.io/qt-5/qt.html#Key-enum
    var KEY_KEYS = [
                Qt.Key_Escape,
                Qt.Key_Tab,
                Qt.Key_Backtab,
                Qt.Key_Backspace,
                Qt.Key_Return,
                Qt.Key_Enter,
                Qt.Key_Insert,
                Qt.Key_Delete,
                Qt.Key_Pause,
                Qt.Key_Print,
                Qt.Key_SysReq,
                Qt.Key_Clear,
                Qt.Key_Home,
                Qt.Key_End,
                Qt.Key_Left,
                Qt.Key_Up,
                Qt.Key_Right,
                Qt.Key_Down,
                Qt.Key_PageUp,
                Qt.Key_PageDown,
                Qt.Key_Shift,
                Qt.Key_Control,
                Qt.Key_Meta,
                Qt.Key_Alt,
                Qt.Key_AltGr,
                Qt.Key_CapsLock,
                Qt.Key_NumLock,
                Qt.Key_ScrollLock]
    var KEY_NAMES = [
                "Escape",
                "Tab",
                "Backtab",
                "Backspace",
                "Return",
                "Enter",
                "Insert",
                "Delete",
                "Pause",
                "Print",
                "SysReq",
                "Clear",
                "Home",
                "End",
                "Left",
                "Up",
                "Right",
                "Down",
                "PageUp",
                "PageDown",
                "Shift",
                "Control",
                "Meta",
                "Alt",
                "AltGr",
                "CapsLock",
                "NumLock",
                "ScrollLock"]

    for (var pos in KEY_KEYS)
    {
        if (e.key === KEY_KEYS[pos])
        {
            return KEY_NAMES[pos]
        }
    }

    console.warn("Unrecognized key: " + e.key)
    return "?"
}

function _getModifiers(e)
{
    // Map type not yet usable
    // https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Map
    // https://bugreports.qt.io/browse/QTBUG-47735
    var MODIFIER_KEYS = [
                Qt.ShiftModifier,
                Qt.ControlModifier,
                Qt.AltModifier,
                Qt.MetaModifier,
                Qt.KeypadModifier,
                Qt.GroupSwitchModifier]
    var MODIFIER_NAMES = [
                "Shift",
                "Ctrl",
                "Alt",
                "Meta",
                "Keypad",
                "GroupSwitchModifier"]

    var modifiers = e.modifiers
    var modifiersOut = []
    for (var pos in MODIFIER_KEYS)
    {
        var key = MODIFIER_KEYS[pos]
        if (e.modifiers & key)
        {
            modifiersOut.push(MODIFIER_NAMES[pos])
            modifiers = modifiers ^ key
        }
    }

    if (modifiers !== Qt.NoModifier)
    {
        console.warn("Unhandled modifiers: " + modifiers)
    }

    return modifiersOut
}

function isPlainKey(keyExpected, event)
{
    return !event.accepted
            && event.modifiers === 0
            && event.key === keyExpected
}

function isCtrlAndKey(keyExpected, event)
{
    return !event.accepted
            && event.modifiers === Qt.ControlModifier
            && event.key === keyExpected
}

function isAltAndKey(keyExpected, event)
{
    return !event.accepted
            && event.modifiers === Qt.AltModifier
            && event.key === keyExpected
}

function nameOfCtrl(isOsx)
{
    if (isOsx) {
        return "⌘"
    }
    else {
        //: What user will find on the key
        return qsTr("Ctrl")
    }
}

function nameOfCtrlAndKey(key, isOsx)
{
    if (isOsx) {
        return nameOfCtrl(isOsx) + key;
    }
    else {
        return nameOfCtrl(isOsx) + "+" + key
    }
}
