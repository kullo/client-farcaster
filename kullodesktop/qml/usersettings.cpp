/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#include "usersettings.h"

#include <chrono>
#include <limits>
#include <QFile>
#include <QMimeDatabase>
#include <QSettings>
#include <QPainter>
#include <QPixmap>
#include <QPrinter>
#include <QPrintDialog>
#include <QImageReader>
#include <QBuffer>

#include <desktoputil/stlqt.h>
#include <desktoputil/kulloclient2qt.h>
#include <desktoputil/qtypestreamers.h>
#include <kulloclient/api/Address.h>
#include <kulloclient/api/MasterKey.h>
#include <kulloclient/api/UserSettings.h>
#include <kulloclient/api_impl/DateTime.h>
#include <kulloclient/util/assert.h>
#include <kulloclient/util/librarylogger.h>
#include <kulloclient/util/misc.h>

namespace KulloDesktop {
namespace Qml {

namespace {
const auto AVATAR_MAX_WIDTH  = 200;
const auto AVATAR_MAX_HEIGHT = 200;
const auto AVATAR_MAX_BYTES  = 24*1024;
const auto SETTING_DOESNT_EXIST = std::numeric_limits<int>::max();
}

UserSettings::UserSettings(const std::shared_ptr<Kullo::Api::Address> &address,
                           const std::shared_ptr<Kullo::Api::MasterKey> &masterKey,
                           QObject *parent)
    : QObject(parent)
    , rawAddress_(address)
    , rawMasterKey_(masterKey)
{
    kulloAssert(rawAddress_);
    kulloAssert(rawMasterKey_);
}

void UserSettings::setUserSettings(
        const std::shared_ptr<Kullo::Api::UserSettings> &userSettings)
{
    kulloAssert(userSettings);
    kulloAssert(userSettings->address()->isEqualTo(rawAddress_));
    kulloAssert(userSettings->masterKey()->isEqualTo(rawMasterKey_));

    settings_ = userSettings;

    emit nameChanged();
    emit organizationChanged();
    emit avatarMimeTypeChanged();
    emit footerChanged();
}

QString UserSettings::address() const
{
    return rawAddress_ ? QString::fromStdString(rawAddress_->toString()) : "";
}

QString UserSettings::masterKeyPem() const
{
    return rawMasterKey_ ? QString::fromStdString(rawMasterKey_->pem()) : "";
}

QString UserSettings::name() const
{
    if (!settings_) return "";
    return QString::fromStdString(settings_->name());
}

void UserSettings::setName(const QString &name)
{
    if (!settings_) return;
    if (settings_->name() == name.toStdString()) return;

    settings_->setName(name.toStdString());
    emit nameChanged();
}

QString UserSettings::organization() const
{
    if (!settings_) return "";
    return QString::fromStdString(settings_->organization());
}

void UserSettings::setOrganization(const QString &organization)
{
    if (!settings_) return;
    if (settings_->organization() == organization.toStdString()) return;

    settings_->setOrganization(organization.toStdString());
    emit organizationChanged();
}

QString UserSettings::avatarMimeType() const
{
    if (!settings_) return "";
    QString mimeType = QString::fromStdString(settings_->avatarMimeType());
    return mimeType;
}

bool UserSettings::tmpAvatarActive() const
{
    return tmpAvatarFileUrl_.isValid();
}

QUrl UserSettings::tmpAvatarFileUrl() const
{
    return tmpAvatarFileUrl_;
}

bool UserSettings::setTmpAvatarFileUrl(const QUrl &tmpAvatarFileUrl)
{
    if (tmpAvatarFileUrl_ == tmpAvatarFileUrl) return false;

    if (tmpAvatarFileUrl.isEmpty())
    {
        tmpAvatarFileUrl_ = tmpAvatarFileUrl;
    }
    else
    {
        tmpAvatarFileUrl_ = tmpAvatarFileUrl;
        if (!loadAvatarFile(tmpAvatarData_, tmpAvatarMimeType_, tmpAvatarFileUrl_)) return false;
    }

    emit tmpAvatarFileUrlChanged();
    emit tmpAvatarActiveChanged();
    return true;
}

QString UserSettings::footer() const
{
    if (!settings_) return "";
    return QString::fromStdString(settings_->footer());
}

void UserSettings::setFooter(QString footer)
{
    if (!settings_) return;
    if (settings_->footer() == footer.toStdString()) return;

    settings_->setFooter(footer.toStdString());
    emit footerChanged();
}

void UserSettings::confirmMasterKeyBackup()
{
    setMasterKeyBackupConfirmed(true);
    Log.i() << "MasterKey backup confirmation set.";
}

void UserSettings::postponeMasterKeyBackupDontRemindBefore(const int seconds)
{
    if (!settings_) return;
    auto delta = std::chrono::seconds{seconds};
    auto dontRemindBefore = Kullo::Api::DateTime::nowUtc() + delta;
    settings_->setNextMasterKeyBackupReminder(dontRemindBefore);
    Log.i() << "MasterKey backup: Don't remind before: " << dontRemindBefore;
}

void UserSettings::printMasterKey(const QString &introText) const
{
    Log.i() << "Opening print dialog ...";
    auto printer = Kullo::make_unique<QPrinter>();
    kulloAssert(printer);

    Log.i() << "Initial printer output format: " << printer->outputFormat();
    Log.i() << "Initial printer page size: " << printer->pageSize();

    // https://bugreports.qt.io/browse/QTBUG-28822?focusedCommentId=228093&page=com.atlassian.jira.plugin.system.issuetabpanels:comment-tabpanel#comment-228093
    printer->setOutputFormat(QPrinter::NativeFormat);

    QPrintDialog printDialog(printer.get());
    printDialog.setOption(QAbstractPrintDialog::PrintToFile, false);
    if (printDialog.exec() == QDialog::Accepted)
    {
        Log.i() << "Printing dialog accepted.";

        QPainter painter;
        if (!painter.begin(printer.get()))
        {
            Log.e() << "Failed to open printer device.";
            return;
        }

        QString printText;
        printText.append(introText + "\n");
        printText.append("\n");
        printText.append(masterKeyPem());

        QRect rect(40, 50, 300, 500);
        painter.drawText(rect, 0, printText);
        painter.end();
        Log.i() << "Painting on printer done.";
    }
    else
    {
        Log.i() << "Printing dialog rejected.";
    }
}

QPixmap UserSettings::avatar() const
{
    if (avatarData().isEmpty()) return QPixmap();

    auto format = mimeTypeToFormat(avatarMimeType());
    if (format.isEmpty())
    {
        Log.w() << "avatarMimeType not one of (image/png, image/jpeg): " << avatarMimeType();
        return QPixmap();
    }

    QPixmap retval;
    if (retval.loadFromData(avatarData(), format.toUtf8().data()))
    {
        return retval;
    }
    return QPixmap();
}

QPixmap UserSettings::tmpAvatar() const
{
    if (tmpAvatarData_.isEmpty()) return QPixmap();

    auto format = mimeTypeToFormat(tmpAvatarMimeType_);
    if (format.isEmpty())
    {
        Log.w() << "tmpAvatarMimeType not one of (image/png, image/jpeg): " << tmpAvatarMimeType_;
        return QPixmap();
    }

    QPixmap retval;
    if (retval.loadFromData(tmpAvatarData_, format.toUtf8().data()))
    {
        return retval;
    }
    return QPixmap();
}

void UserSettings::discardTmpAvatar()
{
    setTmpAvatarFileUrl(QUrl());
}

bool UserSettings::storeTmpAvatar()
{
    if (!settings_ || !tmpAvatarActive()) return false;

    settings_->setAvatar(DesktopUtil::StlQt::toVector(tmpAvatarData_));
    settings_->setAvatarMimeType(tmpAvatarMimeType_.toStdString());
    emit avatarMimeTypeChanged();

    discardTmpAvatar();
    return true;
}

void UserSettings::deleteAvatar()
{
    if (!settings_) return;
    settings_->setAvatar(std::vector<unsigned char>());
    settings_->setAvatarMimeType("");
    emit avatarMimeTypeChanged();

    discardTmpAvatar();
}

bool UserSettings::masterKeyBackupConfirmed() const
{
    if (!settings_) return true;
    return settings_->nextMasterKeyBackupReminder() == boost::none;
}

void UserSettings::setMasterKeyBackupConfirmed(bool confirmed)
{
    if (!settings_) return;
    if (confirmed && settings_->nextMasterKeyBackupReminder())
    {
        settings_->setNextMasterKeyBackupReminder(boost::none);
    }
}

QString UserSettings::masterKeyBackupDontRemindBefore() const
{
    if (!settings_ || !settings_->nextMasterKeyBackupReminder())
    {
        return QString();
    }
    else
    {
        return QString::fromStdString(
                    settings_->nextMasterKeyBackupReminder()->toString());
    }
}

void UserSettings::setMasterKeyBackupDontRemindBefore(const QString &rfc3339time)
{
    if (!settings_) return;
    auto dt = Kullo::Api::DateTime::fromRfc3339(rfc3339time.toStdString());

    if (settings_->nextMasterKeyBackupReminder() != dt)
    {
        settings_->setNextMasterKeyBackupReminder(dt);
    }
}

void UserSettings::storeCredentials(const std::shared_ptr<Kullo::Api::Address> &address, const std::shared_ptr<Kullo::Api::MasterKey> &masterKey)
{
    kulloAssert(address);
    kulloAssert(masterKey);

    auto addressString = QString::fromStdString(address->toString());
    auto masterKeyBlocks = DesktopUtil::StlQt::toQStringList(masterKey->dataBlocks());

    QString groupName = "usersettings-" + addressString;

    QSettings settings;
    settings.beginGroup(groupName);
    settings.setValue("address", addressString);
    settings.setValue("masterKey", masterKeyBlocks);
    settings.endGroup();
}

bool UserSettings::checkAndLoadSetting(const QString key, QVariant::Type type, EmptyAction emptyAction, QVariant &data, const QSettings &settings)
{
    data = settings.value(key, SETTING_DOESNT_EXIST);
    if (data.toInt() == SETTING_DOESNT_EXIST)
    {
        if (emptyAction == EmptyAction::Warn)
        {
            Log.w() << "Settings key does not exist: " << key;
        }
        return false;
    }
    else
    {
        if (!data.canConvert(type))
        {
            Log.w() << "Settings entry for " << key << " is of wrong type: "
                    << data.typeName();
            return false;
        }
        else
        {
            if (emptyAction == EmptyAction::Accept) return true;

            if ((type == QVariant::String && data.toString().isEmpty())
                    || (type == QVariant::ByteArray && data.toByteArray().isEmpty())
                    || (type == QVariant::StringList && data.toStringList().isEmpty()))
            {
                if (emptyAction == EmptyAction::Warn)
                {
                    Log.w() << "Settings entry for " << key << " is but should not be empty.";
                }
                return false;
            }
            return true;
        }
    }
}

bool UserSettings::checkLoadAndRemoveSetting(const QString key, QVariant::Type type, UserSettings::EmptyAction empty, QVariant &data, QSettings &settings)
{
    auto result = checkAndLoadSetting(key, type, empty, data, settings);
    settings.remove(key);
    return result;
}

/*
 * Compresses `scaledImage` below the size of `maxBytes`.
 *
 */
std::pair<QByteArray, QString> UserSettings::compressAvatar(const QImage &scaledImage, const QString &format, const int maxBytes)
{
    kulloAssert(format == "jpeg" || format == "png");
    const int initalQuality = 96;
    int quality = initalQuality;
    QString compressedFormat = format;
    QByteArray avatarData;

    do {
        avatarData.clear();
        QBuffer avatarDataBuffer(&avatarData);
        avatarDataBuffer.open(QIODevice::WriteOnly);

        if (compressedFormat == "png" && quality <= 85)  // Force JPEG compression and re-run
        {
            compressedFormat = "jpeg";
            quality = initalQuality;
        }

        if (compressedFormat == "png")
        {
            scaledImage.save(&avatarDataBuffer, compressedFormat.toLocal8Bit().data());
        }
        else if (compressedFormat == "jpeg")
        {
            scaledImage.save(&avatarDataBuffer, compressedFormat.toLocal8Bit().data(), quality);
        }

        Log.i() << "Compressed Avatar. "
                << "Format: " << compressedFormat << " "
                << "Quality: " << quality << " "
                << "Compressed size (KiB): " << avatarDataBuffer.size() / 1024.0;
        quality -= 2;

        avatarDataBuffer.close();

        if (quality <= 0)  // This should not happen
        {
            Log.f() << "Avatar could not be compressed.";
        }
    } while(avatarData.size() > maxBytes);

    kulloAssert(avatarData.size() <= maxBytes);
    return std::pair<QByteArray, QString>(avatarData, compressedFormat);
}

std::unique_ptr<UserSettings> UserSettings::loadCredentialsForAddress(const QString &addressString)
{
    QSettings settings;

    QStringList allGroups = settings.childGroups();
    Log.i() << "Available configuration groups: " << allGroups;

    QString groupName = "usersettings-" + addressString;
    if (!allGroups.contains(groupName))
    {
        Log.w() << "Requested configuration group not found: " << groupName;
    }
    settings.beginGroup(groupName);

    QVariant data;
    auto masterKeyLoaded = checkAndLoadSetting(
                "masterKey", QVariant::StringList, EmptyAction::Warn, data, settings);
    kulloAssert(masterKeyLoaded); //TODO properly handle missing masterKey

    settings.endGroup();

    const auto address = Kullo::Api::Address::create(addressString.toStdString());
    const auto masterKey = Kullo::Api::MasterKey::createFromDataBlocks(
                DesktopUtil::StlQt::toVector(data.toStringList()));

    return Kullo::make_unique<UserSettings>(address, masterKey);
}

/**
 * @brief Load avatar, cut, compress. Store JPEG or PNG data as well as proper mime type.
 * @param filename The absolute or relative path to the image file
 * @return True on success, false if an error occurred.
 */
bool UserSettings::loadAvatarFile(QByteArray &avatarData, QString &avatarMimeType, const QUrl &filename)
{
    if (!filename.isLocalFile())
    {
        Log.e() << "Avatar source URL " << filename << " is not a local file.";
        return false;
    }

    QImageReader reader(filename.toLocalFile());
    QString format = reader.format();

    if (format == "gif" || format == "bmp" || format == "png")
    {
        format = "png";
    }
    else if (format == "jpeg")
    {
        format = "jpeg";
    }
    else
    {
        Log.w() << "Avatar file format none of (gif, bmp, png, jpeg): " << format;
        return false;
    }

    int originalWidth = reader.size().width();
    int originalHeight = reader.size().height();

    QPoint offset;
    QSize size;
    if (originalWidth < originalHeight) // Portrait
    {
        size = QSize(originalWidth, originalWidth);
        offset = QPoint(0, (originalHeight - size.height()) / 2);
    }
    else
    {
        size = QSize(originalHeight, originalHeight);
        offset = QPoint((originalWidth - size.width()) / 2, 0);
    }
    reader.setClipRect(QRect(offset, size));

    QImage image(size, reader.imageFormat());
    if (!reader.read(&image))
    {
        Log.e() << "Could not read avatar from file: " << reader.fileName() << " "
                << "Error:" << reader.error() << " " << reader.errorString();
        return false;
    }

    QImage scaledImage = scaleDown(image);
    Log.i() << "Cut " << size << " at " << offset << " and scale to " << scaledImage.size();

    std::pair<QByteArray, QString> avatar = compressAvatar(scaledImage, format, AVATAR_MAX_BYTES);
    avatarData = avatar.first;
    avatarMimeType = formatToMimeType(avatar.second);

    return true;
}

std::shared_ptr<Kullo::Api::Address> UserSettings::rawAddress() const
{
    return rawAddress_;
}

std::shared_ptr<Kullo::Api::MasterKey> UserSettings::rawMasterKey() const
{
    return rawMasterKey_;
}

void UserSettings::migrate()
{
    if (!settings_) return;
    QSettings settings;

    auto groupName = QString::fromStdString("usersettings-" + settings_->address()->toString());
    settings.beginGroup(groupName);

    QVariant data;
    if (checkLoadAndRemoveSetting("name", QVariant::String, EmptyAction::Ignore, data, settings))
    {
        setName(data.toString());
    }
    if (checkLoadAndRemoveSetting("organization", QVariant::String, EmptyAction::Accept, data, settings))
    {
        setOrganization(data.toString());
    }
    if (checkLoadAndRemoveSetting("avatarMimeType", QVariant::String, EmptyAction::Accept, data, settings))
    {
        setAvatarMimeType(data.toString());
    }
    if (checkLoadAndRemoveSetting("avatarData", QVariant::ByteArray, EmptyAction::Accept, data, settings))
    {
        auto format = mimeTypeToFormat(avatarMimeType());
        QImage image;
        image.loadFromData(data.toByteArray(), format.toLocal8Bit());

        QImage scaledImage = scaleDown(image);
        if (scaledImage.size() != image.size())
        {
            std::pair<QByteArray, QString> avatar = compressAvatar(scaledImage, format, AVATAR_MAX_BYTES);
            setAvatarData(avatar.first);
            setAvatarMimeType(formatToMimeType(avatar.second));
        }
        else
        {
            setAvatarData(data.toByteArray());
        }
    }
    if (checkLoadAndRemoveSetting("footer", QVariant::String, EmptyAction::Accept, data, settings))
    {
        setFooter(data.toString());
    }
    if (checkLoadAndRemoveSetting("masterKeyBackupConfirmed", QVariant::Bool, EmptyAction::Ignore, data, settings))
    {
        setMasterKeyBackupConfirmed(data.toBool());
    }
    if (checkLoadAndRemoveSetting("masterKeyBackupDontRemindBefore", QVariant::String, EmptyAction::Ignore, data, settings))
    {
        setMasterKeyBackupDontRemindBefore(data.toString());
    }

    settings.endGroup();
}

QStringList UserSettings::masterKeyBlocks()
{
    if (!rawMasterKey_) return QStringList();
    return DesktopUtil::StlQt::toQStringList(rawMasterKey_->dataBlocks());
}

QByteArray UserSettings::avatarData() const
{
    if (!settings_) return QByteArray();
    return DesktopUtil::StlQt::toQByteArray(settings_->avatar());
}

void UserSettings::setAvatarData(const QByteArray &data)
{
    if (!settings_) return;
    const std::vector<unsigned char> dataStd = DesktopUtil::StlQt::toVector(data);
    if (settings_->avatar() == dataStd) return;

    settings_->setAvatar(dataStd);
}

void UserSettings::setAvatarMimeType(const QString &mime)
{
    if (!settings_) return;
    if (settings_->avatarMimeType() == mime.toStdString()) return;

    settings_->setAvatarMimeType(mime.toStdString());
    emit avatarMimeTypeChanged();
}

QImage UserSettings::scaleDown(const QImage &image)
{
    QImage scaledImage;
    if (image.size().width() > AVATAR_MAX_WIDTH ||
            image.size().height() > AVATAR_MAX_HEIGHT)
    {
        scaledImage = image.scaled(
                    QSize(AVATAR_MAX_WIDTH, AVATAR_MAX_HEIGHT),
                    Qt::KeepAspectRatio,
                    Qt::SmoothTransformation);
    }
    else
    {
        scaledImage = image;
    }

    return scaledImage;
}

QString UserSettings::mimeTypeToFormat(const QString &mimeType)
{
    if (mimeType == "image/png")  return "png";
    if (mimeType == "image/jpeg") return "jpeg";
    return "";
}

QString UserSettings::formatToMimeType(const QString &format)
{
    if (format == "png")  return "image/png";
    if (format == "jpeg") return "image/jpeg";
    return "";
}

}
}
