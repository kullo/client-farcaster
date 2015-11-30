/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#include "draft.h"

#include <kulloclient/dao/attachmentdao.h>
#include <kulloclient/dao/draftdao.h>
#include <kulloclient/util/assert.h>
#include <kulloclient/util/librarylogger.h>
#include <kulloclient/util/misc.h>

#include "desktoputil/stlqt.h"
#include "desktoputil/dice/model/client.h"
#include "desktoputil/dice/model/draftattachment.h"

using namespace Kullo::Dao;
using namespace Kullo::Util;

namespace Kullo {
namespace Model {

void Draft::save()
{
    dao_->save();
}

void Draft::clear()
{
    dao_->clear();
    save();
}

id_type Draft::id() const
{
    return dao_->id();
}

lastModified_type Draft::lastModified() const
{
    return dao_->lastModified();
}

Dao::DraftState Draft::state() const
{
    return dao_->state();
}

void Draft::setState(DraftState state)
{
    if (dao_->setState(state))
    {
        emit stateChanged();
    }
}

std::string Draft::text() const
{
    return dao_->text();
}

void Draft::setText(const std::string &text)
{
    if (dao_->setText(text))
    {
        emit textChanged();
    }
}

std::string Draft::footer() const
{
    return dao_->footer();
}

void Draft::setFooter(const std::string &footer)
{
    if (dao_->setFooter(footer))
    {
        emit footerChanged();
    }
}

std::string Draft::senderName() const
{
    return dao_->senderName();
}

void Draft::setSenderName(const std::string &name)
{
    if (dao_->setSenderName(name))
    {
        emit senderNameChanged();
    }
}

std::string Draft::senderOrganization() const
{
    return dao_->senderOrganization();
}

void Draft::setSenderOrganization(const std::string &organization)
{
    if (dao_->setSenderOrganization(organization))
    {
        emit senderOrganizationChanged();
    }
}

std::vector<unsigned char> Draft::senderAvatar() const
{
    return dao_->senderAvatar();
}

void Draft::setSenderAvatar(const std::vector<unsigned char> &data)
{
    if (dao_->setSenderAvatar(data))
    {
        emit senderAvatarChanged();
    }
}

std::string Draft::senderAvatarMimeType() const
{
    return dao_->senderAvatarMimeType();
}

void Draft::setSenderAvatarMimeType(const std::string &mime)
{
    dao_->setSenderAvatarMimeType(mime);
}

std::vector<std::shared_ptr<DraftAttachment> > Draft::attachments()
{
    // lazy loading
    if (!attachmentsLoadedFromDb_)
    {
        auto result = AttachmentDao::allForMessage(IsDraft::Yes, dao_->conversationId(), client_->session());
        while (auto attachmentDao = result->next())
        {
            attachments_.push_back(std::make_shared<DraftAttachment>(attachmentDao.release(), nullptr));
        }
        attachmentsLoadedFromDb_ = true;
    }
    return attachments_;
}

std::shared_ptr<DraftAttachment> Draft::addAttachment(const std::string &path, const std::string &mimeType)
{
    kulloAssert(!path.empty());
    kulloAssert(!mimeType.empty());

    Db::SharedSessionPtr session = client_->session();
    id_type id = AttachmentDao::idForNewDraftAttachment(dao_->conversationId(), session);
    auto attDao = make_unique<AttachmentDao>(session);
    attDao->setDraft(true);
    attDao->setMessageId(dao_->conversationId());
    attDao->setIndex(id);
    attDao->save();

    auto att = std::make_shared<DraftAttachment>(attDao.release(), nullptr);
    att->loadFrom(path);
    att->setMimeType(mimeType);
    att->save();
    attachments_.push_back(att);

    emit attachmentsChanged();
    return att;
}

void Draft::removeAttachment(id_type index)
{
    for (auto iter = attachments_.begin(); iter != attachments_.end(); ++iter)
    {
        auto attachment = *iter;
        if (attachment->index() == index)
        {
            attachment->deletePermanently(); // from database
            attachments_.erase(iter);

            emit attachmentsChanged();
            break;
        }
    }
}

void Draft::prepareToSend()
{
    Log.d() << "Draft::prepareToSend()";
    setState(DraftState::Sending);
    setFooter(client_->userSettings().footer);
    setSenderName(client_->userSettings().name);
    setSenderOrganization(client_->userSettings().organization);
    setSenderAvatar(client_->userSettings().avatarData);
    setSenderAvatarMimeType(client_->userSettings().avatarMimeType);
    save();
    Log.d() << "End Draft::prepareToSend()";
}

Draft::Draft(Client *client, id_type convId, QObject *parent)
    : QObject(parent),
      client_(client)
{
    kulloAssert(parent == nullptr);
    kulloAssert(client_);

    dao_ = DraftDao::load(convId, client_->session());
    if (!dao_)
    {
        dao_.reset(new DraftDao(client_->session()));
        dao_->setConversationId(convId);
    }
}

Draft::Draft(Client *client, DraftDao *dao, QObject *parent)
    : QObject(parent),
      client_(client),
      dao_(dao)
{
    kulloAssert(client_);
    kulloAssert(dao_);
}

Draft::~Draft()
{
}

void Draft::reload()
{
    // Clear attachments. Attachments are reloaded from DB
    // on next attachments() call.
    attachments_.clear();
    attachmentsLoadedFromDb_ = false;

    // Reload data. Attention: unsaved changes are discarded
    id_type convId = dao_->conversationId();
    dao_ = DraftDao::load(convId, client_->session());
    kulloAssert(dao_);

    Log.d() << "Draft reloaded. Emitting Draft::modified()";
    emit modified();
}

}
}
