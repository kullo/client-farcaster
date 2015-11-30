/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#include "draftattachment.h"

#include <iterator>

#include <kulloclient/dao/attachmentdao.h>
#include <kulloclient/crypto/hasher.h>
#include <kulloclient/util/assert.h>
#include <kulloclient/util/exceptions.h>
#include <kulloclient/util/filesystem.h>

#include "desktoputil/stlqt.h"
#include "desktoputil/dice/model/conversation.h"

using namespace Kullo::Util;

namespace Kullo {
namespace Model {

DraftAttachment::DraftAttachment(Dao::AttachmentDao *dao, QObject *parent)
    : QObject(parent),
      dao_(dao)
{
    kulloAssert(parent == nullptr);
}

DraftAttachment::~DraftAttachment()
{
}

void DraftAttachment::save()
{
    dao_->save();
}

std::string DraftAttachment::filename() const
{
    return dao_->filename();
}

void DraftAttachment::setFilename(const std::string &filename)
{
    if (dao_->setFilename(filename))
    {
        emit filenameChanged();
    }
}

id_type DraftAttachment::index() const
{
    return dao_->index();
}

std::string DraftAttachment::mimeType() const
{
    return dao_->mimeType();
}

void DraftAttachment::setMimeType(const std::string &mimeType)
{
    if (dao_->setMimeType(mimeType))
    {
        emit mimeTypeChanged();
    }
}

std::size_t DraftAttachment::size() const
{
    return dao_->size();
}

std::string DraftAttachment::hash() const
{
    return dao_->hash();
}

std::string DraftAttachment::note() const
{
    return dao_->note();
}

void DraftAttachment::setNote(const std::string &note)
{
    if (dao_->setNote(note))
    {
        emit noteChanged();
    }
}

std::vector<unsigned char> DraftAttachment::content() const
{
    return dao_->content();
}

void DraftAttachment::loadFrom(const std::string &path)
{
    std::vector<unsigned char> data;

    try
    {
        data = Util::Filesystem::getContent(path);
    }
    catch (...)
    {
        std::throw_with_nested(FilesystemError("Error importing draft attachment"));
    }

    dao_->setSize(data.size());
    dao_->setHash(Crypto::Hasher::sha512Hex(data));
    std::string filename = Util::Filesystem::filename(path);
    dao_->setFilename(filename);
    dao_->save();

    dao_->setContent(data);

    emit changed();
}

void DraftAttachment::saveTo(const std::string &path) const
{
    try
    {
        Util::Filesystem::putContent(path, dao_->content());
    }
    catch (...)
    {
        std::throw_with_nested(FilesystemError("Error saving draft attachment"));
    }
}

bool DraftAttachment::isDeleted() const
{
    return dao_->deleted();
}

void DraftAttachment::deletePermanently()
{
    dao_->deletePermanently();
}

}
}
