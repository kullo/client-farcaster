/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#pragma once

#include <atomic>
#include <utility>
#include <QDateTime>
#include <QObject>
#include <QStringList>
#include <QUrl>
#include <QVariant>

#include <desktoputil/dice/dice-forwards.h>
#include <kulloclient/kulloclient-forwards.h>

class QSettings;

namespace KulloDesktop {
namespace Qml {

class UserSettingsModel : public QObject
{
    Q_OBJECT

public:
    enum AllowEmpty {
        NA, // Not Applicable
        No,
        Yes
    };

    explicit UserSettingsModel(QObject *parent = 0);
    explicit UserSettingsModel(Kullo::Util::UserSettings *userSettings, QObject *parent);

    Q_INVOKABLE void load(const QString &address);

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    QString name() const;
    void setName(QString name);

    Q_PROPERTY(QString address READ address WRITE setAddress NOTIFY addressChanged)
    QString address() const;
    void setAddress(QString address);

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

    Q_PROPERTY(QString masterKeyPem READ masterKeyPem WRITE setMasterKeyPem NOTIFY masterKeyPemChanged)
    QString masterKeyPem() const;
    void setMasterKeyPem(const QString &key);

    /// Set masterKeyBackupConfirmed to true and save to disc
    /// In QML, use this function instead of masterKeyBackupConfirmed = true
    /// because it saves changes.
    Q_INVOKABLE void confirmMasterKeyBackup();

    /// Set masterKeyBackupDontRemindBefore to now()+seconds and save to disc
    /// In QML, use this function instead of masterKeyBackupDontRemindBefore = 'XY'
    /// because it saves changes.
    Q_INVOKABLE void postponeMasterKeyBackupDontRemindBefore(const int seconds);

    Q_INVOKABLE void printMasterKey(const QString &introText) const;

    Q_INVOKABLE bool loginCredentialsStored() const;

    Q_INVOKABLE void reset(const QString &address, const QString &masterKeyPem = "");

    QPixmap avatar() const;

    QPixmap tmpAvatar() const;

    Q_INVOKABLE void discardTmpAvatar();
    Q_INVOKABLE bool storeTmpAvatar();
    Q_INVOKABLE void deleteAvatar();

    // Non-QML methods

    bool masterKeyBackupConfirmed() const;
    void setMasterKeyBackupConfirmed(bool confirmed);

    QString masterKeyBackupDontRemindBefore() const;
    void setMasterKeyBackupDontRemindBefore(const QString &rfc3339time);

    static bool loadAvatarFile(QByteArray &avatarData, QString &avatarMimeType, const QUrl &filename);

    Kullo::Util::UserSettings *rawUserSettings() const;

signals:
    void nameChanged();
    void addressChanged();
    void organizationChanged();
    void avatarMimeTypeChanged();
    void tmpAvatarActiveChanged();
    void tmpAvatarFileUrlChanged();
    void footerChanged();
    void masterKeyPemChanged();
    void mutedChanged();

public slots:
    void save();

private:
    QStringList masterKeyBlocks();
    bool checkAndLoadSetting(const QString key, QVariant::Type type, AllowEmpty empty, QVariant &data, const QSettings &settings);
    static std::pair<QByteArray, QString> compressAvatar(const QImage &scaledImage, const QString &format, const int maxBytes);
    QByteArray avatarData() const;
    void setAvatarData(const QByteArray &data);
    void setAvatarMimeType(const QString &mime);

    Kullo::Util::UserSettings *settings_ = nullptr;
    std::atomic<bool> currentlyLoading_;

    QUrl tmpAvatarFileUrl_;
    QByteArray tmpAvatarData_;
    QString tmpAvatarMimeType_;

    Q_DISABLE_COPY(UserSettingsModel)
};

}
}
