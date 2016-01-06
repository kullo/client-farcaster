/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
pragma Singleton
import QtQuick 2.4

import Kullo 1.0

QtObject {
    // Kullo colours
    property color black: "#1A171B"
    property color gray: "#585759"
    property color grayLight: "#838282"
    property color baige: "#E3E0CF"
    property color baigeLight: "#F1ECDD"
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

    property color conversationsListBackground: "#FFFFFF"
    property color messagesListBackground: "#CCCCCC"
    property color answerColumnBackground: "#999999"

    property color messageHeaderBackground: "#707070"
    property color messageHeaderForeground: "#fafafa"
    property color messageHeaderForegroundSecondary: "#99fafafa" // 60% opacity
    property color messageBackground: conversationsListBackground
    property color messageText: black
    property color messageFooter: "#666666"

    property string buttonBackgroundColor: blue
    property string buttonBackgroundColorDisabled: Utils.setSaturation(buttonBackgroundColor, 0.3)
    property string buttonHoverColor: "#22ffffff"
    property string buttonTextColor: "#ffffff"

    property color bannerBackground: "#80ffffff"
    property color errorBannerBackground: red
    property color errorBannerTextColor: "#ffffff"

    // point size appears smaller on OS X
    // https://forum.qt.io/topic/26663/different-os-s-different-font-sizes
    // OS X: 72 ppi; Windows: 96 ppi
    // So we scale up on OS X
    property var fontSize: QtObject {
        property real sp: Os.osx
                          ? 1.25 * (Devicesettings.fontSizeUserFactorPreview / 100.0)
                          : 1.00 * (Devicesettings.fontSizeUserFactorPreview / 100.0)
        property real shortcutOverlayHeadline: 20*sp
        property real shortcutOverlayKey: 10*sp
        property real answerTextInput: 14*sp
        property real bigTextInput: 14*sp
        property real normal: 11*sp
        property real small: 10*sp
        property real attachmentFilename: 9*sp
        property real attachmentFilesize: 8.5*sp
        property real conversationListTitle: small
        property real conversationListCounter: 10*sp
        property real conversationIntroAdresses: 16*sp
        property real conversationIntroNormal: small
        property real overlay: 26*sp
        property real message: Devicesettings.messagesFontPreview.pointSize*sp
        property real messageHeaderPrimary: 13*sp
        property real messageHeaderSecondary: 10*sp
        property real loggingIn: 25*sp
        property real loginKulloAddressInput: bigTextInput
        property real kulloVersion: 27*sp
        property real masterKeyBackupHeadline: 20*sp
        property real registrationKeygenCheckmark: 21*sp
        property real registrationKulloAdressUsername: bigTextInput
        property real registrationKulloAdressDomain: bigTextInput + 1*sp
        property real registrationChallengeAnswerInput: bigTextInput
    }
}
