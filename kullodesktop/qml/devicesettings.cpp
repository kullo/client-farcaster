/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
#include "devicesettings.h"

#include <QSettings>
#include <desktoputil/kulloversion.h>
#include <desktoputil/qtypestreamers.h>
#include <desktoputil/osdetection.h>
#include <kulloclient/util/assert.h>
#include <kulloclient/util/librarylogger.h>
#include <kulloclient/util/misc.h>

namespace KulloDesktop {
namespace Qml {

namespace {
const int MAIN_WINDOW_DEFAULT_WIDTH  = 1000;
const int MAIN_WINDOW_DEFAULT_HEIGHT =  600;
const auto MAIN_WINDOW_DEFAULT_MAXIMIZED = false;
const int MAIN_WINDOW_MIN_WIDTH  = 800;
const int MAIN_WINDOW_MIN_HEIGHT = 400;
const auto DEFAULT_UPDATE_BRANCH = QStringLiteral("important");
const auto DEFAULT_CLOSE_TO_TRAY = true;
const auto DEFAULT_FONT_SIZE_FACTOR = int{100};
const auto ANSWER_COLUMN_WIDTH_DEFAULT = int{300};
const auto ANSWER_COLUMN_WIDTH_MINIMUM = int{150};


/*
 * Linux:
 *   - Liberation Serif
 *
 * Windows:
 *   - Cambria (specifically designed for on-screen reading; Windows Vista, Windows 7, Windows 8, Windows 8.1, Microsoft Office 2007, Microsoft Office 2010, Microsoft Office 2013, Microsoft Office 2008 for Mac and Microsoft Office 2011 for Mac)
 *
 * OS X:
 *   - Times
 *
 * Fallbacks:
 *   - Times New Roman (fallback)
 *
 * See also:
 * http://www.granneman.com/webdev/coding/css/fonts-and-formatting/default-fonts/
 *
 */
#if defined(K_OS_LINUX)
const auto DEFAULT_MESSAGES_FONT = QFont("Liberation Serif", 14);
#elif defined(K_OS_WINDOWS)
const auto DEFAULT_MESSAGES_FONT = QFont("Cambria", 14);
#elif defined(K_OS_OSX)
const auto DEFAULT_MESSAGES_FONT = QFont("Times", 14);
#endif

}

DeviceSettings::DeviceSettings(QObject *parent)
    : QObject(parent)
{
}

DeviceSettings::~DeviceSettings()
{
}

void DeviceSettings::migrate()
{
    int settingsVersion = 0;
    QSettings settings;
    settings.beginGroup("global");
    QVariant data = settings.value("settingsVersion");
    if (data.isValid()) settingsVersion = data.toUInt();
    settings.endGroup();

    Log.i() << "Current settingsVersion: "
            << settingsVersion;

    switch(settingsVersion)
    {
    case 0:
        Log.i() << "Migrate settings to version: 1";

#ifdef QT_NO_DEBUG
        QSettings oldSettings("Kullo", "Kullo Client");
#else
        QSettings oldSettings("Kullo", "Kullo Client Debug");
#endif
        oldSettings.setFallbacksEnabled(false);
        Log.i() << "Delete config entries: " << oldSettings.allKeys();
        oldSettings.clear();

        setSettingsVersion(1);
    }
}

int DeviceSettings::mainWindowMinWidth() const
{
    return MAIN_WINDOW_MIN_WIDTH;
}

int DeviceSettings::mainWindowMinHeight() const
{
    return MAIN_WINDOW_MIN_HEIGHT;
}

int DeviceSettings::getMainWindowWidth()
{
    auto out = int{MAIN_WINDOW_DEFAULT_WIDTH};

    if (!mainWindowSettings_) initMainWindowSettings();
    QVariant data = mainWindowSettings_->value("global/mainWindowWidth");
    if (data.isValid()) out = data.toInt();

    return out;
}

void DeviceSettings::setMainWindowWidth(int width)
{
    if (width < MAIN_WINDOW_MIN_WIDTH) return;
    if (getMainWindowWidth() == width) return;

    if (!mainWindowSettings_) initMainWindowSettings();
    mainWindowSettings_->setValue("global/mainWindowWidth", width);
}

int DeviceSettings::getMainWindowHeight()
{
    auto out = int{MAIN_WINDOW_DEFAULT_HEIGHT};

    if (!mainWindowSettings_) initMainWindowSettings();
    QVariant data = mainWindowSettings_->value("global/mainWindowHeight");
    if (data.isValid()) out = data.toInt();

    return out;
}

void DeviceSettings::setMainWindowHeight(int height)
{
    if (height < MAIN_WINDOW_MIN_HEIGHT) return;
    if (getMainWindowHeight() == height) return;

    if (!mainWindowSettings_) initMainWindowSettings();
    mainWindowSettings_->setValue("global/mainWindowHeight", height);
}

bool DeviceSettings::getMainWindowMaximized()
{
    auto out = MAIN_WINDOW_DEFAULT_MAXIMIZED;

    if (!mainWindowSettings_) initMainWindowSettings();
    QVariant data = mainWindowSettings_->value("global/mainWindowMaximized");
    if (data.isValid()) out = data.toBool();

    return out;
}

void DeviceSettings::setMainWindowMaximized(bool maximized)
{
    if (getMainWindowMaximized() == maximized) return;

    Log.d() << "Maximized: " << maximized;
    if (!mainWindowSettings_) initMainWindowSettings();
    mainWindowSettings_->setValue("global/mainWindowMaximized", maximized);
}

int DeviceSettings::fontSizeUserFactor()
{
    if (!fontSizeUserFactor_)
    {
        int fontSizeFactor = DEFAULT_FONT_SIZE_FACTOR;
        QSettings settings;
        settings.beginGroup("global");
        QVariant data = settings.value("fontSizeUserFactor");
        if (data.isValid()) fontSizeFactor = data.toInt();
        settings.endGroup();
        fontSizeUserFactor_ = Kullo::make_unique<int>(fontSizeFactor);
    }

    return *fontSizeUserFactor_;
}

void DeviceSettings::setFontSizeUserFactor(int factor)
{
    kulloAssert(factor > 0 && factor <= 200);

    if (fontSizeUserFactor() == factor) return;

    QSettings settings;
    settings.beginGroup("global");
    settings.setValue("fontSizeUserFactor", factor);
    settings.endGroup();

    fontSizeUserFactor_ = Kullo::make_unique<int>(factor);
    emit fontSizeUserFactorChanged();
}

int DeviceSettings::fontSizeUserFactorPreview()
{
    if (!fontSizeUserFactorPreview_)
    {
        fontSizeUserFactorPreview_ = Kullo::make_unique<int>(fontSizeUserFactor());
    }

    return *fontSizeUserFactorPreview_;
}

void DeviceSettings::setFontSizeUserFactorPreview(int factor)
{
    if (fontSizeUserFactorPreview_ && *fontSizeUserFactorPreview_ == factor) return;

    fontSizeUserFactorPreview_ = Kullo::make_unique<int>(factor);
    emit fontSizeUserFactorPreviewChanged();
}

bool DeviceSettings::muted() const
{
    bool muted = false;
    QSettings settings;
    settings.beginGroup("global");
    QVariant data = settings.value("muted");
    if (data.isValid()) muted = data.toBool();
    settings.endGroup();
    return muted;
}

void DeviceSettings::setMuted(bool muted)
{
    if (this->muted() == muted) return;

    QSettings settings;
    settings.beginGroup("global");
    settings.setValue("muted", muted);
    settings.endGroup();

    emit mutedChanged();
}

QFont DeviceSettings::messagesFont()
{
    if (!messagesFont_)
    {
        QFont font = DEFAULT_MESSAGES_FONT;
        QSettings settings;
        settings.beginGroup("global");
        QVariant data = settings.value("messagesFont");
        if (data.isValid()) font = data.value<QFont>();
        settings.endGroup();
        messagesFont_ = Kullo::make_unique<QFont>(font);
    }

    return *messagesFont_;
}

void DeviceSettings::setMessagesFont(const QFont font)
{
    if (messagesFont() == font) return;

    QSettings settings;
    settings.beginGroup("global");
    settings.setValue("messagesFont", font);
    settings.endGroup();

    messagesFont_ = Kullo::make_unique<QFont>(font);
    emit messagesFontChanged();
}

QFont DeviceSettings::messagesFontPreview()
{
    if (!messagesFontPreview_)
    {
        messagesFontPreview_ = Kullo::make_unique<QFont>(messagesFont());
    }

    return *messagesFontPreview_;
}

void DeviceSettings::setMessagesFontPreview(const QFont font)
{
    if (messagesFontPreview_ && *messagesFontPreview_ == font) return;

    messagesFontPreview_ = Kullo::make_unique<QFont>(font);
    emit messagesFontPreviewChanged();
}

bool DeviceSettings::closeToTray() const
{
    bool closeToTray = DEFAULT_CLOSE_TO_TRAY;
    QSettings settings;
    settings.beginGroup("global");
    QVariant data = settings.value("closeToTray");
    if (data.isValid()) closeToTray = data.toBool();
    settings.endGroup();
    return closeToTray;
}

void DeviceSettings::setCloseToTray(bool closeToTray)
{
    if (this->closeToTray() == closeToTray) return;

    QSettings settings;
    settings.beginGroup("global");
    settings.setValue("closeToTray", closeToTray);
    settings.endGroup();

    emit closeToTrayChanged(closeToTray);
}

int DeviceSettings::answerColumnWidth()
{
    if (!answerColumnWidth_)
    {
        int storedWidth = ANSWER_COLUMN_WIDTH_DEFAULT;
        QSettings settings;
        settings.beginGroup("global");
        QVariant data = settings.value("answerColumnWidth");
        if (data.isValid()) storedWidth = data.toInt();
        settings.endGroup();
        answerColumnWidth_ = boost::make_optional<int>(std::max(storedWidth, ANSWER_COLUMN_WIDTH_MINIMUM));
    }

    return *answerColumnWidth_;
}

void DeviceSettings::setAnswerColumnWidth(int width)
{
    if (width < ANSWER_COLUMN_WIDTH_MINIMUM) width = ANSWER_COLUMN_WIDTH_MINIMUM;

    if (answerColumnWidth() == width) return;

    answerColumnWidth_ = boost::make_optional<int>(width);

    QSettings settings;
    settings.beginGroup("global");
    settings.setValue("answerColumnWidth", width);
    settings.endGroup();

    emit answerWidthChanged();
}

QString DeviceSettings::updateLane() const
{
    auto branch = DEFAULT_UPDATE_BRANCH;

    QSettings settings;
    settings.beginGroup("global");
    QVariant data = settings.value("updateBranch");
    if (data.isValid()) branch = data.toString();
    settings.endGroup();

    kulloAssert(branch == "important" || branch == "all");

    return branch;
}

void DeviceSettings::setUpdateLane(const QString &lane)
{
    kulloAssert(lane == "important" || lane == "all");

    if (this->updateLane() == lane) return;

    QSettings settings;
    settings.beginGroup("global");
    settings.setValue("updateBranch", lane);
    settings.endGroup();

    emit updateLaneChanged();
}

QString DeviceSettings::silencedUpdate() const
{
    QString silencedUpdate;

    QSettings settings;
    settings.beginGroup("global");
    QVariant data = settings.value("silencedUpdate");
    if (data.isValid()) silencedUpdate = data.toString();
    settings.endGroup();

    if (!silencedUpdate.isEmpty()) {
        DesktopUtil::KulloVersion(silencedUpdate.toStdString()); // validate format
    }

    return silencedUpdate;
}

void DeviceSettings::setSilencedUpdate(const QString &version)
{
    if (!version.isEmpty()) {
        DesktopUtil::KulloVersion(version.toStdString()); // validate format
    }

    if (this->silencedUpdate() == version) return;

    QSettings settings;
    settings.beginGroup("global");
    settings.setValue("silencedUpdate", version);
    settings.endGroup();

    emit silencedUpdateChanged();
}

QString DeviceSettings::activeUser()
{
    QString activeUser;
    QSettings settings;
    settings.beginGroup("global");
    QVariant data = settings.value("activeUser");
    if (data.isValid()) activeUser = data.toString();
    settings.endGroup();
    return activeUser;
}

QString DeviceSettings::lastActiveUser()
{
    QString lastActiveUser;
    QSettings settings;
    settings.beginGroup("global");
    QVariant data = settings.value("lastActiveUser");
    if (data.isValid()) lastActiveUser = data.toString();
    settings.endGroup();
    return lastActiveUser;
}

QStringList DeviceSettings::usersList() const
{
    const QString prefix = "usersettings-";
    const int prefixLen = 13;

    QStringList out;
    QSettings settings;
    QStringList keys = settings.childGroups();
    for (QString key : keys)
    {
         if (key.indexOf(prefix) == 0)
         {
             QString address = key.right(key.length()-prefixLen);
             out << address;
         }
    }
    out.sort();
    return out;
}

void DeviceSettings::setActiveUser(const QString &address)
{
    if (this->activeUser() == address) return;

    QSettings settings;
    settings.beginGroup("global");
    settings.setValue("activeUser", address);
    settings.endGroup();

    emit activeUserChanged();
}

void DeviceSettings::setLastActiveUser(const QString &address)
{
    if (this->lastActiveUser() == address) return;

    QSettings settings;
    settings.beginGroup("global");
    settings.setValue("lastActiveUser", address);
    settings.endGroup();

    emit lastActiveUserChanged();
}

void DeviceSettings::setSettingsVersion(int version)
{
    QSettings settings;
    settings.beginGroup("global");
    settings.setValue("settingsVersion", version);
    settings.endGroup();
}

void DeviceSettings::initMainWindowSettings()
{
    mainWindowSettings_ = Kullo::make_unique<QSettings>();
}

}
}
