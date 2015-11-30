/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#pragma once

#include <QObject>
#include <memory>

#include <kulloclient/kulloclient-forwards.h>
#include <kulloclient/types.h>
#include <kulloclient/util/misc.h>

#include "desktoputil/dice/dice-forwards.h"

namespace Kullo {
namespace Model {

/**
 * @brief Describes a file that is attached to an IMessage
 *
 * For attachments of drafts, have a look at IDraftAttachment.
 */
class Attachment : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Get the attachment's message ID, e.g. 54
     * @return The attachment's message ID
     */
    id_type messageId() const;

    /**
     * @brief Get the attachment's original filename, e.g. "cv.pdf"
     * @return The attachment's filename
     */
    std::string filename() const;

    /**
     * @brief Get the attachment's MIME type, e.g. "application/pdf"
     * @return The attachment's MIME type
     */
    std::string mimeType() const;

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

    /**
     * @brief Save the attachment to the given path, e.g. "/home/joe/Documents/cv.pdf"
     * @param path The path to the file were the attachment should be written. Must contain a filename!
     * @throw FilesystemException if an error occurs while saving.
     */
    void saveTo(const std::string &path) const;

    /**
     * @brief Get attachment's raw content
     * @return The attachment's content in binary format
     */
    std::vector<unsigned char> content() const;

    /**
     * @brief Constructs an Attachment that uses an existing AttachmentDao.
     *
     * Sets the parent of the Dao to the newly constructed object.
     *
     * @param dao The dao to be used
     * @param parent See QObject::parent()
     */
    Attachment(Dao::AttachmentDao *dao, QObject *parent = 0);

    ~Attachment();

    /// Deletes a message's draft from DB.
    void deletePermanently();

private:
    std::unique_ptr<Dao::AttachmentDao> dao_;

    K_DISABLE_COPY(Attachment);
};

}
}
