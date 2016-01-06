/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <memory>
#include <vector>
#include <QObject>

#include <kulloclient/kulloclient-forwards.h>
#include <kulloclient/types.h>
#include <kulloclient/dao/enums.h>
#include <kulloclient/util/misc.h>

#include "desktoputil/dice/dice-forwards.h"

namespace Kullo {
namespace Model {

/// Describes a message draft.
class Draft : public QObject
{
    Q_OBJECT

public:
    /// Saves any state changes of the draft to DB.
    void save();

    /// Delete all contents of this draft.
    void clear();

    /// Get the draft ID, as assigned by the server, or 0 if it wasn's synced yet.
    id_type id() const;

    /**
     * @brief Get the modification timestamp as assigned by the server.
     *
     * The format is microseconds since 1970-01-01 00:00 UTC (i.e. timestamp * 1,000,000)
     */
    lastModified_type lastModified() const;

    /// Get the draft's state.
    Dao::DraftState state() const;

    /// Set the draft's state.
    void setState(Dao::DraftState state);

    /// Get the main content part of the draft.
    std::string text() const;

    /// Set the main content part of the draft.
    void setText(const std::string &text);

    /// Get the footer/imprint part of the draft.
    std::string footer() const;

    /// Set the footer/imprint part of the draft.
    void setFooter(const std::string &footer);

    /// Get the sender's name
    std::string senderName() const;

    /// Set the sender's name
    void setSenderName(const std::string &name);

    /// Get the sender's organization
    std::string senderOrganization() const;

    /// Set the sender's organization
    void setSenderOrganization(const std::string &organization);

    /// Get the sender's avatar data
    std::vector<unsigned char> senderAvatar() const;

    /// Set the sender's avatar data
    void setSenderAvatar(const std::vector<unsigned char> &data);

    /// Get the sender's avatar mime type
    std::string senderAvatarMimeType() const;

    /// Set the sender's avatar mime type
    void setSenderAvatarMimeType(const std::string &mime);

    /**
     * @brief Get the attachments for the draft.
     * @return A potentially empty list of attachments.
     */
    std::vector<std::shared_ptr<DraftAttachment>> attachments();

    /**
     * @brief Add a new attachment to the draft.
     * @param path Path to the file that should be loaded.
     * @return The new attachment.
     */
    std::shared_ptr<DraftAttachment> addAttachment(const std::string &path, const std::string &mimeType);

    /**
     * @brief Removes an attachment from a draft.
     * @param index The index of the attachment to be removed.
     */
    void removeAttachment(id_type index);

    /**
     * @brief Enqueue the draft for sending.
     *
     * This changes the state to STATE_SENDING. To really send the draft,
     * start a sync.
     */
    void prepareToSend();

    /**
     * @brief Tries to load a draft from DB.
     *
     * If no draft can be found for this conversation ID, an unlinked object with
     * the given conversation ID is constructed.
     *
     * @param convId The conversation ID to be searched for
     * @param parent See QObject::parent()
     */
    Draft(Client *client, id_type convId, QObject *parent = 0);

    /**
     * @brief Loads a Draft from DB
     * @param query Must point at a valid record from which the data is loaded.
     * @param parent See QObject::parent()
     */
    Draft(Client *client, Dao::DraftDao *dao, QObject *parent = 0);

    ~Draft();

    /**
     * @brief Discards unsaved changes and reloads the data of this object.
     *
     * This is called when the syncer has modified the DB representation of
     * the draft.
     */
    void reload();

signals:
    /// Emitted when the whole draft was modified.
    void modified();

    /// Emitted when the text was changed.
    void textChanged();

    /// Emitted when the footer was changed.
    void footerChanged();

    // Emitted when the sender's name changed.
    void senderNameChanged();

    // Emitted when the sender's organization changed.
    void senderOrganizationChanged();

    // Emitted when the sender's avatar changed.
    void senderAvatarChanged();

    /// Emitted when the list of attachments was changed.
    void attachmentsChanged();

    /// Emitted when the state was changed.
    void stateChanged();

private:
    Client *client_ = nullptr;
    std::unique_ptr<Dao::DraftDao> dao_;
    std::vector<std::shared_ptr<DraftAttachment>> attachments_;
    bool attachmentsLoadedFromDb_ = false;

    K_DISABLE_COPY(Draft);
};

}
}
