/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#pragma once

#include <memory>
#include <QObject>

#include <kulloclient/kulloclient-forwards.h>
#include <kulloclient/types.h>
#include <kulloclient/util/misc.h>

#include "desktoputil/dice/dice-forwards.h"

namespace Kullo {
namespace Model {

/**
 * @brief Describes a file that is attached to an IDraft.
 *
 * For attachments of messages, have a look at Attachment.
 */
class DraftAttachment : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a DraftAttachment that uses an existing AttachmentDao.
     *
     * Sets the parent of the Dao to the newly constructed object.
     *
     * @param dao The dao to be used
     * @param parent See QObject::parent()
     */
    DraftAttachment(Dao::AttachmentDao *dao, QObject *parent = 0);

    ~DraftAttachment();

    /// Saves any changes of the attachment to DB.
    void save();

    /**
     * @brief Get the attachment's original filename, e.g. "cv.pdf"
     * @return The attachment's filename, without path
     */
    std::string filename() const;

    /**
     * @brief Set a new filename, e.g. "cv.pdf"
     * @param filename The attachment's filename, without path
     */
    void setFilename(const std::string &filename);

    /**
     * @brief Get the attachment's index (zero-based, e.g. 3)
     * @return The attachment's index
     */
    id_type index() const;

    /**
     * @brief Get the attachment's MIME type, e.g. "application/pdf"
     * @return The attachment's MIME type
     */
    std::string mimeType() const;

    /// Set a new MIME type, e.g. "application/pdf"
    void setMimeType(const std::string &mimeType);

    /**
     * @brief Get the attachment's size in bytes
     * @return The attachment's size in bytes
     */
    std::size_t size() const;

    /**
     * @brief Get the SHA-512 hash of the content
     * @return The SHA-512 hash of the content as a hex string
     */
    std::string hash() const;

    /**
     * @brief Get the textual note that can be added to the attachment by the sender, e.g. "applicant's CV"
     * @return The attachment's note
     */
    std::string note() const;

    /// Set a new note which is usually displayed along with the attachment.
    void setNote(const std::string &note);

    /**
     * @brief Get attachment's raw content
     * @return The attachment's content in binary format
     */
    std::vector<unsigned char> content() const;

    /**
     * @brief Overwrite the current attachment from the given path, e.g. "/home/joe/Documents/cv.pdf"
     * @param path The path to the file which should replace the current
     *             attachment. Must contain a filename!
     * @throw FilesystemException if an error occurs while saving.
     *
     * The filename, mimeType and size is updated automatically.
     */
    void loadFrom(const std::string &path);

    /**
     * @brief Save the attachment to the given path, e.g. "/home/joe/Documents/cv.pdf"
     * @param path The path to the file were the attachment should be written. Must contain a filename!
     * @throw FilesystemException if an error occurs while saving.
     */
    void saveTo(const std::string &path) const;

    /**
     * @brief isDeleted
     * @return true iff deletePermanently() was called. In this case,
     *         don't use this object any more!
     */
    bool isDeleted() const;

    /// Deletes the attachment draft from the DB.
    void deletePermanently();

signals:
    /// Emitted when the whole draft attachment was changed.
    void changed();

    /// Emitted when the filename was changed.
    void filenameChanged();

    /// Emitted when the MIME type was changed.
    void mimeTypeChanged();

    /// Emitted when the note was changed.
    void noteChanged();

private:
    std::unique_ptr<Dao::AttachmentDao> dao_;

    K_DISABLE_COPY(DraftAttachment);
};

}
}
