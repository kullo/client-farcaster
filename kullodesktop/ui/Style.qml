/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
pragma Singleton
import QtQuick 2.4

import Kullo 1.0

QtObject {
    // Kullo colours
    property color black: "#1A171B"
    property color gray: "#585759"
    property color grayLight: "#838282"
    property color beige: "#E3E0CF"
    property color beigeLight: "#F1ECDD"
    property color blue: "#044C98"
    property color blueLight: "#186FB1"
    property color green: "#5FA027"
    property color greenLight: "#96BD0D"
    property color turquoise: "#009F95"
    property color turquoiseLight: "#54B9BB"
    property color orange: "#E88D03"
    property color orangeLight: "#F7B234"
    property color purple: "#771C7F"
    property color purpleLight: "#91127D"
    property color red: "#B90B34"
    property color redLight: "#D33447"
    property color white: "#FFFFFF"

    property color welcomeAccountsBackground: "#fafafa"
    property color welcomeAccountsForeground: black
    property color welcomeAccountsHighlightedBackground: blue
    property color welcomeAccountsHighlightedForeground: white
    property color welcomeAccountsSecondaryHighlightedBackground: "#ccc"
    property color welcomeAccountsSecondaryHighlightedForeground: black

    property color conversationsListBackground: "#FFFFFF"
    property color messagesListBackground: "#CCCCCC"
    property color answerColumnBackground: "#999999"

    property color leftToolbarBackground: "#e6e6e6"

    property color messageHeaderBackground: "#707070"
    property color messageHeaderForeground: "#fafafa"
    property color messageHeaderForegroundSecondary: "#99fafafa" // 60% opacity
    property color messageBackground: conversationsListBackground
    property color messageText: black
    property color messageFooter: "#666666"

    property color profileHeadline: Utils.setAlpha(black, 0.9)
    property color profileHeadlineSecondary: Utils.setAlpha(black, 0.7)

    property color answerHeaderBackground: "transparent"
    property color answerHeaderForeground: Utils.setAlpha(black, 0.9)
    property color answerHeaderForegroundSecondary: Utils.setAlpha(black, 0.7)

    property string buttonBackgroundColor: blue
    property string buttonBackgroundColorDisabled: Utils.setSaturation(buttonBackgroundColor, 0.3)
    property string buttonHoverColor: "#22ffffff"
    property string buttonTextColor: "#ffffff"

    property color bannerBackground: "#ddffffff"
    property int bannerBorderRadius: 7 * Hdpi.FontScalingFactor
    property color errorBannerBackground: red
    property color errorBannerTextColor: "#ffffff"

    // point size appears smaller on OS X
    // https://forum.qt.io/topic/26663/different-os-s-different-font-sizes
    // OS X: 72 ppi; Windows: 96 ppi
    // So we scale up on OS X
    property var fontSize: QtObject {
        property real sp: Os.osx
                          ? 1.25 * (InnerApplication.deviceSettings.fontSizeUserFactorPreview / 100.0)
                          : 1.00 * (InnerApplication.deviceSettings.fontSizeUserFactorPreview / 100.0)
        property real shortcutOverlayHeadline: 20*sp
        property real shortcutOverlayKey: 10*sp
        property real answerTextInput: InnerApplication.deviceSettings.messagesFontPreview.pointSize*sp
        property real bigTextInput: 14*sp
        property real big: 13*sp
        property real normal: 11*sp
        property real small: 10*sp
        property real attachmentFilename: 9*sp
        property real attachmentFilesize: 8.5*sp
        property real conversationListTitle: small
        property real conversationListCounter: 10*sp
        property real conversationIntroAdresses: 16*sp
        property real conversationIntroNormal: small
        property real message: InnerApplication.deviceSettings.messagesFontPreview.pointSize*sp
        property real messageHeaderPrimary: 13*sp
        property real messageHeaderSecondary: 10*sp
        property real loggingIn: 25*sp
        property real loginKulloAddressInput: bigTextInput
        property real kulloVersion: 35*sp
        property real masterKeyBackupHeadline: 20*sp
        property real registrationKeygenCheckmark: 21*sp
        property real registrationKulloAdressUsername: bigTextInput
        property real registrationKulloAdressDomain: bigTextInput + 1*sp
        property real registrationChallengeAnswerInput: bigTextInput
        property real toast: 26*sp
    }
}
