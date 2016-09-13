/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <atomic>
#include <memory>
#include <utility>
#include <QDateTime>
#include <QObject>
#include <QStringList>
#include <QUrl>
#include <QVariant>
#include <boost/optional.hpp>

#include <kulloclient/kulloclient-forwards.h>
#include <kulloclient/api/Address.h>
#include <kulloclient/api/DateTime.h>
#include <kulloclient/api/MasterKey.h>

class QSettings;

namespace KulloDesktop {
namespace Qml {

class UserSettingsModel : public QObject
{
    Q_OBJECT

public:
    enum EmptyAction {
        Ignore, // silently ignore empty value
        Warn,   // ignore empty value, print a warning
        Accept  // accept an empty value
    };

    explicit UserSettingsModel(QObject *parent = nullptr);
    void setUserSettings(const std::shared_ptr<Kullo::Api::UserSettings> &userSettings);

    Q_INVOKABLE void load(const QString &address);
    void migrate();

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    QString name() const;
    void setName(QString name);

    Q_PROPERTY(QString address READ address NOTIFY addressChanged)
    QString address() const;

    Q_PROPERTY(QString organization READ organization WRITE setOrganization NOTIFY organizationChanged)
    QString organization() const;
    void setOrganization(QString organization);

    Q_PROPERTY(QString avatarMimeType READ avatarMimeType NOTIFY avatarMimeTypeChanged)
    QString avatarMimeType() const;

    Q_PROPERTY(bool tmpAvatarActive READ tmpAvatarActive NOTIFY tmpAvatarActiveChanged)
    bool tmpAvatarActive() const;

    Q_PROPERTY(QUrl tmpAvatarFileUrl READ tmpAvatarFileUrl WRITE setTmpAvatarFileUrl NOTIFY tmpAvatarFileUrlChanged)
    QUrl tmpAvatarFileUrl() const;
    Q_INVOKABLE bool setTmpAvatarFileUrl(const QUrl &tmpAvatarFileUrl);

    Q_PROPERTY(QString footer READ footer WRITE setFooter NOTIFY footerChanged)
    QString footer() const;
    void setFooter(QString footer);

    Q_PROPERTY(QString masterKeyPem READ masterKeyPem NOTIFY masterKeyPemChanged)
    QString masterKeyPem() const;

    /// Set masterKeyBackupConfirmed to true and save to disk
    /// In QML, use this function instead of masterKeyBackupConfirmed = true
    /// because it saves changes.
    Q_INVOKABLE void confirmMasterKeyBackup();

    /// Set masterKeyBackupDontRemindBefore to now()+seconds and save to disk
    /// In QML, use this function instead of masterKeyBackupDontRemindBefore = 'XY'
    /// because it saves changes.
    Q_INVOKABLE void postponeMasterKeyBackupDontRemindBefore(const int seconds);

    Q_INVOKABLE void printMasterKey(const QString &introText) const;

    Q_INVOKABLE bool loginCredentialsStored() const;

    QPixmap avatar() const;

    QPixmap tmpAvatar() const;

    Q_INVOKABLE void discardTmpAvatar();
    Q_INVOKABLE bool storeTmpAvatar();
    Q_INVOKABLE void deleteAvatar();

    // Non-QML methods
    void reset(const std::shared_ptr<Kullo::Api::Address> &address, const std::shared_ptr<Kullo::Api::MasterKey> &masterKey);

    bool masterKeyBackupConfirmed() const;
    void setMasterKeyBackupConfirmed(bool confirmed);

    QString masterKeyBackupDontRemindBefore() const;
    void setMasterKeyBackupDontRemindBefore(const QString &rfc3339time);

    static bool loadAvatarFile(QByteArray &avatarData, QString &avatarMimeType, const QUrl &filename);

    std::shared_ptr<Kullo::Api::Address> rawAddress() const;
    std::shared_ptr<Kullo::Api::MasterKey> rawMasterKey() const;

signals:
    void nameChanged();
    void addressChanged();
    void organizationChanged();
    void avatarMimeTypeChanged();
    void tmpAvatarActiveChanged();
    void tmpAvatarFileUrlChanged();
    void footerChanged();
    void masterKeyPemChanged();

public slots:
    void save();

private:
    QStringList masterKeyBlocks();
    bool checkAndLoadSetting(const QString key, QVariant::Type type, EmptyAction empty, QVariant &data, const QSettings &settings);
    bool checkLoadAndRemoveSetting(const QString key, QVariant::Type type, EmptyAction empty, QVariant &data, QSettings &settings);
    static std::pair<QByteArray, QString> compressAvatar(const QImage &scaledImage, const QString &format, const int maxBytes);
    QByteArray avatarData() const;
    void setAvatarData(const QByteArray &data);
    void setAvatarMimeType(const QString &mime);
    static QImage scaleDown(const QImage &image);
    static QString mimeTypeToFormat(const QString &mimeType);
    static QString formatToMimeType(const QString &format);

    std::shared_ptr<Kullo::Api::Address> rawAddress_;
    std::shared_ptr<Kullo::Api::MasterKey> rawMasterKey_;
    std::shared_ptr<Kullo::Api::UserSettings> settings_;
    std::atomic<bool> currentlyLoading_;

    QUrl tmpAvatarFileUrl_;
    QByteArray tmpAvatarData_;
    QString tmpAvatarMimeType_;

    Q_DISABLE_COPY(UserSettingsModel)
};

}
}
