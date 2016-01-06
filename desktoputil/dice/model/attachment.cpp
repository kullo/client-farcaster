/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#include "attachment.h"

#include <kulloclient/dao/attachmentdao.h>
#include <kulloclient/util/assert.h>
#include <kulloclient/util/exceptions.h>
#include <kulloclient/util/filesystem.h>

#include "desktoputil/stlqt.h"
#include "desktoputil/dice/model/conversation.h"

using namespace Kullo::Util;

namespace Kullo {
namespace Model {

id_type Attachment::messageId() const
{
    return dao_->messageId();
}

std::string Attachment::filename() const
{
    return dao_->filename();
}

std::string Attachment::mimeType() const
{
    return dao_->mimeType();
}

std::size_t Attachment::size() const
{
    return dao_->size();
}

std::string Attachment::hash() const
{
    return dao_->hash();
}

std::string Attachment::note() const
{
    return dao_->note();
}

void Attachment::saveTo(const std::string &path) const
{
    try
    {
        Util::Filesystem::putContent(path, dao_->content());
    }
    catch (...)
    {
        std::throw_with_nested(FilesystemError("Error saving attachment"));
    }
}

std::vector<unsigned char> Attachment::content() const
{
    return dao_->content();
}

void Attachment::deletePermanently()
{
    dao_->deletePermanently();
}

Attachment::Attachment(Dao::AttachmentDao *dao, QObject *parent)
    : QObject(parent),
      dao_(dao)
{
    kulloAssert(parent == nullptr);
}

Attachment::~Attachment()
{
}

}
}
