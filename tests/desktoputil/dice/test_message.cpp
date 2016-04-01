/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#include <memory>
#include <vector>

#include <kulloclient/dao/attachmentdao.h>
#include <kulloclient/dao/messagedao.h>
#include <kulloclient/dao/participantdao.h>
#include <kulloclient/util/misc.h>

#include <desktoputil/dice/model/attachment.h>
#include <desktoputil/dice/model/message.h>
#include <desktoputil/dice/model/participant.h>

#include "modeltest.h"

using namespace testing;
using namespace Kullo;

namespace {
    struct TestData
    {
        const id_type id = 42;
        const id_type conversationId = 23;
        const lastModified_type lastModified = 1111222211112222;
        const std::string senderAddressStr = "test#kullo.net";
        const bool deleted = false;
        const std::string dateSent = "2001-01-20T13:37:42Z";
        const std::string dateReceived = "2525-02-25T23:42:05Z";
        const std::string text = "Hello, world.";
        const std::string footer = "Goodbye, world.";
        const std::vector<unsigned char> symmetricKey = std::initializer_list<unsigned char>({23, 42, 5});
        const bool read = true;
        const bool done = false;
    };
}

class MessageModel : public ModelTest
{
public:
    std::unique_ptr<Dao::MessageDao> getExampleMessageDao()
    {
        TestData data;

        // create necessary sender record
        Dao::ParticipantDao sender(Util::KulloAddress(data.senderAddressStr), session_);
        sender.setMessageId(data.id);
        sender.save();

        auto dao = make_unique<Dao::MessageDao>(session_);
        dao->setId(data.id);
        dao->setConversationId(data.conversationId);
        dao->setLastModified(data.lastModified);
        dao->setSender(data.senderAddressStr);
        dao->setDeleted(data.deleted);
        dao->setDateSent(data.dateSent);
        dao->setDateReceived(data.dateReceived);
        dao->setText(data.text);
        dao->setFooter(data.footer);
        dao->setSymmetricKey(data.symmetricKey);
        dao->setState(Dao::MessageState::Read, data.read);
        dao->setState(Dao::MessageState::Done, data.done);
        dao->save(Dao::CreateOld::No);
        return dao;
    }

    std::vector<std::vector<std::string>> getTestAttachments() const
    {
        std::vector<std::vector<std::string>> testdata;
        std::vector<std::string> row(5);

        row[0] = "test file number 1.txt";
        row[1] = "text/plain";
        row[2] = "8";
        row[3] = "Hello Berlin!";
        row[4] = "fa585d89c851dd338a70dcf535aa2a92fee7836dd6aff1226583e88e0996293f16bc009c652826e0fc5c706695a03cddce372f139eff4d13959da6f1f5d3eabe";
        testdata.push_back(row);

        row[0] = "test file number 2.txt";
        row[1] = "text/plain";
        row[2] = "8";
        row[3] = "Hello Berlin!";
        row[4] = "fa585d89c851dd338a70dcf535aa2a92fee7836dd6aff1226583e88e0996293f16bc009c652826e0fc5c706695a03cddce372f139eff4d13959da6f1f5d3eabe";
        testdata.push_back(row);

        row[0] = "test file number 3.txt";
        row[1] = "text/plain";
        row[2] = "8";
        row[3] = "Hello Berlin!";
        row[4] = "fa585d89c851dd338a70dcf535aa2a92fee7836dd6aff1226583e88e0996293f16bc009c652826e0fc5c706695a03cddce372f139eff4d13959da6f1f5d3eabe";
        testdata.push_back(row);

        return testdata;
    }
};

TEST_F(MessageModel, insertsNewMessageMinimal)
{
    auto dao = make_unique<Dao::MessageDao>(session_);
    dao->setId(123);
    dao->save(Dao::CreateOld::No);

    auto msg = make_unique<Model::Message>(client_.get(), dao.release());
    ASSERT_THAT(msg, NotNull());
    EXPECT_THAT(msg->id(), Eq(id_type{123}));
}

TEST_F(MessageModel, insertsNewMessageFull)
{
    TestData data;

    {
        // Create from dao
        auto dao = make_unique<Dao::MessageDao>(session_);
        dao->setId(data.id);
        dao->setLastModified(data.lastModified);
        dao->setConversationId(data.conversationId);
        dao->setText(data.text);
        dao->setFooter(data.footer);
        dao->setSender(data.senderAddressStr);
        dao->setDeleted(data.deleted);
        dao->setDateSent(data.dateSent);
        dao->setDateReceived(data.dateReceived);
        dao->setState(Dao::MessageState::Read, data.read);
        dao->setState(Dao::MessageState::Done, data.done);
        dao->save(Dao::CreateOld::No);

        auto participant = make_unique<Dao::ParticipantDao>(Util::KulloAddress(data.senderAddressStr), session_);
        participant->setMessageId(data.id);
        participant->save();

        auto msg = make_unique<Model::Message>(client_.get(), dao.release());
        ASSERT_THAT(msg, NotNull());
        EXPECT_THAT(msg->id(), Eq(data.id));
        EXPECT_THAT(msg->lastModified(), Eq(data.lastModified));
        EXPECT_THAT(msg->conversationId(), Eq(data.conversationId));
        EXPECT_THAT(msg->text(), Eq(data.text));
        EXPECT_THAT(msg->footer(), Eq(data.footer));
        EXPECT_THAT(msg->sender()->address(), Eq(Util::KulloAddress(data.senderAddressStr)));
        EXPECT_THAT(msg->isDeleted(), Eq(data.deleted));
        EXPECT_THAT(msg->dateSent(), Eq(Util::DateTime(data.dateSent)));
        EXPECT_THAT(msg->dateReceived(), Eq(Util::DateTime(data.dateReceived)));
        EXPECT_THAT(msg->state(Dao::MessageState::Read), Eq(data.read));
        EXPECT_THAT(msg->state(Dao::MessageState::Done), Eq(data.done));
        EXPECT_THAT(msg->state(Dao::MessageState::Unread), Eq(!data.read));
        EXPECT_THAT(msg->state(Dao::MessageState::Undone), Eq(!data.done));
    }

    {
        // Load by id
        auto msg = make_unique<Model::Message>(client_.get(), data.id);
        ASSERT_THAT(msg, NotNull());
        EXPECT_THAT(msg->id(), Eq(data.id));
        EXPECT_THAT(msg->lastModified(), Eq(data.lastModified));
        EXPECT_THAT(msg->conversationId(), Eq(data.conversationId));
        EXPECT_THAT(msg->text(), Eq(data.text));
        EXPECT_THAT(msg->footer(), Eq(data.footer));
        EXPECT_THAT(msg->sender()->address(), Eq(Util::KulloAddress(data.senderAddressStr)));
        EXPECT_THAT(msg->isDeleted(), Eq(data.deleted));
        EXPECT_THAT(msg->dateSent(), Eq(Util::DateTime(data.dateSent)));
        EXPECT_THAT(msg->dateReceived(), Eq(Util::DateTime(data.dateReceived)));
        EXPECT_THAT(msg->state(Dao::MessageState::Read), Eq(data.read));
        EXPECT_THAT(msg->state(Dao::MessageState::Done), Eq(data.done));
        EXPECT_THAT(msg->state(Dao::MessageState::Unread), Eq(!data.read));
        EXPECT_THAT(msg->state(Dao::MessageState::Undone), Eq(!data.done));
    }
}

TEST_F(MessageModel, addAttachment)
{
    auto dao = getExampleMessageDao();
    auto msg = make_unique<Model::Message>(client_.get(), dao.release());
    ASSERT_THAT(msg, NotNull());

    {
        auto att = make_unique<Dao::AttachmentDao>(session_);
        att->setDraft(false);
        att->setMessageId(msg->id());
        att->setIndex(0);
        att->setFilename("test123.txt");
        att->setMimeType("text/plain");
        att->setSize(8);
        att->setNote("Too lacy");
        att->setHash("fa585d89c851dd338a70dcf535aa2a92fee7836dd6aff1226583e88e0996293f16bc009c652826e0fc5c706695a03cddce372f139eff4d13959da6f1f5d3eabe");
        att->save();
    }

    EXPECT_THAT(msg->attachments().size(), Eq(size_t{1}));
    std::shared_ptr<Model::Attachment> msgAttachment;
    EXPECT_NO_THROW(msgAttachment = msg->attachments().at(0));
    EXPECT_THAT(msgAttachment->filename(), Eq("test123.txt"));
    EXPECT_THAT(msgAttachment->mimeType(), Eq("text/plain"));
    EXPECT_THAT(msgAttachment->size(), Eq(size_t{8}));
    EXPECT_THAT(msgAttachment->note(), Eq("Too lacy"));
    EXPECT_THAT(msgAttachment->hash(), Eq("fa585d89c851dd338a70dcf535aa2a92fee7836dd6aff1226583e88e0996293f16bc009c652826e0fc5c706695a03cddce372f139eff4d13959da6f1f5d3eabe"));
}

TEST_F(MessageModel, addAttachmentsAndDeleteMessage)
{
    int messageId;

    { // life of msg1
        auto dao1 = getExampleMessageDao();
        auto msg1 = make_unique<Model::Message>(client_.get(), dao1.release());
        ASSERT_THAT(msg1, NotNull());

        messageId = msg1->id();

        EXPECT_THAT(msg1->attachments().size(), Eq(size_t{0}));

        std::vector<std::vector<std::string>> testdata = getTestAttachments();
        for (int i = 0; i < 3; ++i)
        {
            auto att = make_unique<Dao::AttachmentDao>(session_);
            att->setDraft(false);
            att->setMessageId(msg1->id());
            att->setIndex(i);
            att->setFilename(testdata[i][0]);
            att->setMimeType(testdata[i][1]);
            att->setSize(atoi(testdata[i][2].c_str()));
            att->setNote(testdata[i][3]);
            att->setHash(testdata[i][4]);
            att->save();
        }

        EXPECT_THAT(msg1->attachments().size(), Eq(size_t{0}));
    }

    { // life of msg2
        auto msg2 = make_unique<Model::Message>(client_.get(), messageId);
        ASSERT_THAT(msg2, NotNull());

        EXPECT_THAT(msg2->attachments().size(), Eq(size_t{3}));

        msg2->deletePermanently();
    }

    { // life of msg3
        auto msg3 = make_unique<Model::Message>(client_.get(), messageId);
        ASSERT_THAT(msg3, NotNull());

        EXPECT_THAT(msg3->isDeleted(), Eq(true));
        EXPECT_THAT(msg3->attachments().size(), Eq(size_t{0}));
    }
}

TEST_F(MessageModel, compareMessages)
{
    TestData data;

    // msg1 < msg2 = msg3 < msg4

    auto dao1 = make_unique<Dao::MessageDao>(session_);
    dao1->setConversationId(data.conversationId);
    dao1->setId(101);
    dao1->setDateSent("2024-01-01T19:30:04+02:00");
    dao1->save(Dao::CreateOld::No);
    auto msg1 = make_unique<Model::Message>(client_.get(), dao1.release());

    auto dao2 = make_unique<Dao::MessageDao>(session_);
    dao2->setConversationId(data.conversationId);
    dao2->setId(102);
    dao2->setDateSent("2024-01-01T19:30:05+02:00");
    dao2->save(Dao::CreateOld::No);
    auto msg2 = make_unique<Model::Message>(client_.get(), dao2.release());

    EXPECT_LT(*msg1, *msg2);

    auto dao3 = make_unique<Dao::MessageDao>(session_);
    dao3->setConversationId(data.conversationId);
    dao3->setId(103);
    dao3->setDateSent("2024-01-01T18:30:05+01:00");
    dao3->save(Dao::CreateOld::No);
    auto msg3 = make_unique<Model::Message>(client_.get(), dao3.release());

    EXPECT_FALSE(*msg2 < *msg3);
    EXPECT_FALSE(*msg2 > *msg3);
    EXPECT_LT(*msg1, *msg3);

    auto dao4 = make_unique<Dao::MessageDao>(session_);
    dao4->setConversationId(data.conversationId);
    dao4->setId(104);
    dao4->setDateSent("2024-01-01T18:30:06+01:00");
    dao4->save(Dao::CreateOld::No);
    auto msg4 = make_unique<Model::Message>(client_.get(), dao4.release());

    EXPECT_LT(*msg1, *msg4);
    EXPECT_LT(*msg2, *msg4);
    EXPECT_LT(*msg3, *msg4);
}
