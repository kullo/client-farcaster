/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#pragma once

#include <memory>
#include <QFont>
#include <QObject>
#include <QStringList>
#include <boost/optional.hpp>

class QSettings;

namespace KulloDesktop {
namespace Qml {

class DeviceSettings : public QObject
{
    Q_OBJECT

public:
    explicit DeviceSettings(QObject *parent = nullptr);
    ~DeviceSettings();

    void migrate();

    Q_PROPERTY(int mainWindowMinWidth READ mainWindowMinWidth NOTIFY mainWindowMinWidthChanged)
    int mainWindowMinWidth() const;

    Q_PROPERTY(int mainWindowMinHeight READ mainWindowMinHeight NOTIFY mainWindowMinHeightChanged)
    int mainWindowMinHeight() const;

    // use explicit getter and setter functions to avoid signal terror
    Q_INVOKABLE int getMainWindowWidth();
    Q_INVOKABLE void setMainWindowWidth(int width);

    Q_INVOKABLE int getMainWindowHeight();
    Q_INVOKABLE void setMainWindowHeight(int height);

    Q_INVOKABLE bool getMainWindowMaximized();
    Q_INVOKABLE void setMainWindowMaximized(bool maximized);

    Q_PROPERTY(int fontSizeUserFactor READ fontSizeUserFactor WRITE setFontSizeUserFactor NOTIFY fontSizeUserFactorChanged)
    int fontSizeUserFactor();
    void setFontSizeUserFactor(int factor);

    Q_PROPERTY(int fontSizeUserFactorPreview READ fontSizeUserFactorPreview WRITE setFontSizeUserFactorPreview NOTIFY fontSizeUserFactorPreviewChanged)
    int fontSizeUserFactorPreview();
    void setFontSizeUserFactorPreview(int factor);

    Q_PROPERTY(bool muted READ muted WRITE setMuted NOTIFY mutedChanged)
    bool muted() const;
    void setMuted(bool muted);

    Q_PROPERTY(QFont messagesFont READ messagesFont WRITE setMessagesFont NOTIFY messagesFontChanged)
    QFont messagesFont();
    void setMessagesFont(const QFont font);

    Q_PROPERTY(QFont messagesFontPreview READ messagesFontPreview WRITE setMessagesFontPreview NOTIFY messagesFontPreviewChanged)
    QFont messagesFontPreview();
    void setMessagesFontPreview(const QFont font);

    Q_PROPERTY(bool closeToTray READ closeToTray WRITE setCloseToTray NOTIFY closeToTrayChanged)
    bool closeToTray() const;
    void setCloseToTray(bool closeToTray);

    Q_PROPERTY(int answerColumnWidth READ answerColumnWidth WRITE setAnswerColumnWidth NOTIFY answerWidthChanged)
    int answerColumnWidth();
    void setAnswerColumnWidth(int width);

    Q_PROPERTY(QString updateLane READ updateLane WRITE setUpdateLane NOTIFY updateLaneChanged)
    QString updateLane() const;
    void setUpdateLane(const QString &lane);

    Q_PROPERTY(QString silencedUpdate READ silencedUpdate WRITE setSilencedUpdate NOTIFY silencedUpdateChanged)
    QString silencedUpdate() const;
    void setSilencedUpdate(const QString &version);

    Q_PROPERTY(QString activeUser READ activeUser NOTIFY activeUserChanged)
    QString activeUser();

    Q_PROPERTY(QString lastActiveUser READ lastActiveUser NOTIFY lastActiveUserChanged)
    QString lastActiveUser();

    Q_INVOKABLE QStringList usersList() const;

    void setActiveUser(const QString &address);
    void setLastActiveUser(const QString &address);

signals:
    void fontSizeUserFactorChanged();
    void fontSizeUserFactorPreviewChanged();
    void mutedChanged();
    void messagesFontChanged();
    void messagesFontPreviewChanged();
    void closeToTrayChanged(bool value);
    void answerWidthChanged();
    void updateLaneChanged();
    void silencedUpdateChanged();
    void activeUserChanged();
    void lastActiveUserChanged();
    void usersListChanged();

    // unused
    void mainWindowMinWidthChanged();
    void mainWindowMinHeightChanged();

private:
    void setSettingsVersion(int version);
    void inline initMainWindowSettings();

    std::unique_ptr<int> fontSizeUserFactor_;
    std::unique_ptr<int> fontSizeUserFactorPreview_;

    std::unique_ptr<QFont> messagesFont_;
    std::unique_ptr<QFont> messagesFontPreview_;

    boost::optional<int> answerColumnWidth_;

    // Must be initialized when Application is set up,
    // i.e. in first setMainWindowHeight or setMainWindowHeight call
    std::unique_ptr<QSettings> mainWindowSettings_;
};

}
}
