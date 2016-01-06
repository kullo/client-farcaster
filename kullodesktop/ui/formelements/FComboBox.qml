/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Controls 1.3

ComboBox {
    property bool _FORM_TYPE_INPUT: true

    function findParentForm()
    {
        var elem = parent
        do {
            if (elem._FORM_TYPE_FORM)
            {
                return elem
            }
            elem = elem.parent
        } while (elem)
        return null
    }

    function primaryAction()
    {
        var form = findParentForm()
        if (!form)
        {
            console.warn("Parent form not found.")
        }
        else
        {
            form.submit()
        }
    }

    Keys.onReturnPressed: primaryAction()
    Keys.onEnterPressed: primaryAction()
}
