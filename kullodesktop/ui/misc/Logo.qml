/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
import QtQuick 2.4

import "../native"

NativeImage {
    source: "/resources/scalable/logo.svg"
    width: 150 * Hdpi.FontScalingFactor
    height: 45 * Hdpi.FontScalingFactor
    smooth: true
}
