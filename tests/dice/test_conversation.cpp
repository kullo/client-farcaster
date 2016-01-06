/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#include <memory>
#include <vector>

#include <kulloclient/dao/conversationdao.h>
#include <kulloclient/dao/messagedao.h>
#include <kulloclient/dao/participantdao.h>
#include <kulloclient/util/misc.h>
#include <kulloclient/types.h>

#include <desktoputil/dice/model/conversation.h>
#include <desktoputil/dice/model/message.h>

#include "modeltest.h"

using namespace testing;
using namespace Kullo;

class ConversationModel : public ModelTest
{
public:
    ConversationModel()
        : ModelTest()
        , addrA_("senderA#kullo.net")
        , addrB_("senderB#kullo.net")
        , addrC_("senderC#kullo.net")
        , addrD_("senderD#kullo.net")
    {}

    std::unique_ptr<Model::Message> createMessage(Kullo::id_type id, const std::string &senderAddress, Kullo::id_type conversationId)
    {
        auto msgDao = make_unique<Dao::MessageDao>(session_);
        msgDao->setConversationId(conversationId);
        msgDao->setId(id);
        msgDao->setSender(senderAddress);
        switch(id)
        {
        case 0:
            msgDao->setDateSent("2024-01-01T19:30:04+02:00");
            msgDao->setState(Dao::MessageState::Read, false);
            msgDao->setState(Dao::MessageState::Done, false);
            break;
        case 1:
            msgDao->setDateSent("2024-01-01T19:30:05+02:00");
            msgDao->setState(Dao::MessageState::Read, true);
            msgDao->setState(Dao::MessageState::Done, false);
            break;
        case 20:
            msgDao->setDateSent("2024-01-01T18:30:05+01:00");
            msgDao->setState(Dao::MessageState::Read, false);
            msgDao->setState(Dao::MessageState::Done, true);
            break;
        case Kullo::ID_MAX:
            msgDao->setDateSent("2024-01-01T18:30:06+01:00");
            msgDao->setState(Dao::MessageState::Read, true);
            msgDao->setState(Dao::MessageState::Done, true);
            break;
        default:
            kulloAssert(false);
        }
        msgDao->save(Dao::CreateOld::No);
        return make_unique<Model::Message>(client_.get(), msgDao.release());
    }

    const Util::KulloAddress addrA_;
    const Util::KulloAddress addrB_;
    const Util::KulloAddress addrC_;
    const Util::KulloAddress addrD_;

    const Kullo::id_type msgId1_ = 0;
    const Kullo::id_type msgId2_ = 1;
    const Kullo::id_type msgId3_ = 20;
    const Kullo::id_type msgId4_ = Kullo::ID_MAX;
};

TEST_F(ConversationModel, compareConversations)
{
    // msg1 < msg2 = msg3 < msg4
    // conv1 < conv2 = conv3 < conv4

    {
        auto partDao1 = make_unique<Dao::ParticipantDao>(addrA_, session_);
        auto partDao2 = make_unique<Dao::ParticipantDao>(addrB_, session_);
        auto partDao3 = make_unique<Dao::ParticipantDao>(addrC_, session_);
        auto partDao4 = make_unique<Dao::ParticipantDao>(addrD_, session_);
        partDao1->setMessageId(msgId1_);
        partDao2->setMessageId(msgId2_);
        partDao3->setMessageId(msgId3_);
        partDao4->setMessageId(msgId4_);
        partDao1->save();
        partDao2->save();
        partDao3->save();
        partDao4->save();
    }

    auto convDao1 = make_unique<Dao::ConversationDao>(session_);
    auto convDao2 = make_unique<Dao::ConversationDao>(session_);
    auto convDao3 = make_unique<Dao::ConversationDao>(session_);
    auto convDao4 = make_unique<Dao::ConversationDao>(session_);

    convDao1->setParticipants(std::set<Util::KulloAddress>{addrA_});
    convDao2->setParticipants(std::set<Util::KulloAddress>{addrB_});
    convDao3->setParticipants(std::set<Util::KulloAddress>{addrC_});
    convDao4->setParticipants(std::set<Util::KulloAddress>{addrD_});

    convDao1->save();
    convDao2->save();
    convDao3->save();
    convDao4->save();

    auto msg1 = createMessage(msgId1_, addrA_.toString(), convDao1->id());
    auto msg2 = createMessage(msgId2_, addrB_.toString(), convDao2->id());
    auto msg3 = createMessage(msgId3_, addrC_.toString(), convDao3->id());
    auto msg4 = createMessage(msgId4_, addrD_.toString(), convDao4->id());
    K_UNUSED(msg1);
    K_UNUSED(msg2);
    K_UNUSED(msg3);
    K_UNUSED(msg4);

    Model::Conversation conv1(client_.get(), convDao1.release());
    Model::Conversation conv2(client_.get(), convDao2.release());
    Model::Conversation conv3(client_.get(), convDao3.release());
    Model::Conversation conv4(client_.get(), convDao4.release());
    conv1.loadMessages();
    conv2.loadMessages();
    conv3.loadMessages();
    conv4.loadMessages();

    EXPECT_LT(conv1, conv2);

    EXPECT_FALSE(conv2 < conv3);
    EXPECT_FALSE(conv2 > conv3);

    EXPECT_LT(conv1, conv4);
    EXPECT_LT(conv2, conv4);
    EXPECT_LT(conv3, conv4);
}

TEST_F(ConversationModel, compareConversationsWidth2Messages)
{
    // msg1 < msg2 = msg3 < msg4
    // convA = (msg1, msg4)
    // convB = (msg2, msg3)
    // (msg2, msg3) < (msg1, msg4)
    // convB < convA

    {
        auto partDao1 = make_unique<Dao::ParticipantDao>(addrA_, session_);
        auto partDao2 = make_unique<Dao::ParticipantDao>(addrB_, session_);
        auto partDao3 = make_unique<Dao::ParticipantDao>(addrB_, session_);
        auto partDao4 = make_unique<Dao::ParticipantDao>(addrA_, session_);
        partDao1->setMessageId(msgId1_);
        partDao2->setMessageId(msgId2_);
        partDao3->setMessageId(msgId3_);
        partDao4->setMessageId(msgId4_);
        partDao1->save();
        partDao2->save();
        partDao3->save();
        partDao4->save();
    }

    auto convDaoA = make_unique<Dao::ConversationDao>(session_);
    auto convDaoB = make_unique<Dao::ConversationDao>(session_);

    convDaoA->setParticipants(std::set<Util::KulloAddress>{addrA_});
    convDaoB->setParticipants(std::set<Util::KulloAddress>{addrB_});

    convDaoA->save();
    convDaoB->save();

    auto msg1 = createMessage(msgId1_, addrA_.toString(), convDaoA->id());
    auto msg2 = createMessage(msgId2_, addrB_.toString(), convDaoB->id());
    auto msg3 = createMessage(msgId3_, addrB_.toString(), convDaoB->id());
    auto msg4 = createMessage(msgId4_, addrA_.toString(), convDaoA->id());
    K_UNUSED(msg1);
    K_UNUSED(msg2);
    K_UNUSED(msg3);
    K_UNUSED(msg4);

    Model::Conversation convA(client_.get(), convDaoA.release());
    Model::Conversation convB(client_.get(), convDaoB.release());
    convA.loadMessages();
    convB.loadMessages();

    EXPECT_LT(convB, convA);
}

TEST_F(ConversationModel, compareConversationsWidth3Messages)
{
    // msg1 < msg2 = msg3 < msg4
    // convA = (msg4)
    // convB = (msg1, msg2, msg3)
    // (msg1, msg2, msg3) < (msg4)
    // convB < convA

    {
        auto partDao1 = make_unique<Dao::ParticipantDao>(addrB_, session_);
        auto partDao2 = make_unique<Dao::ParticipantDao>(addrB_, session_);
        auto partDao3 = make_unique<Dao::ParticipantDao>(addrB_, session_);
        auto partDao4 = make_unique<Dao::ParticipantDao>(addrA_, session_);
        partDao1->setMessageId(msgId1_);
        partDao2->setMessageId(msgId2_);
        partDao3->setMessageId(msgId3_);
        partDao4->setMessageId(msgId4_);
        partDao1->save();
        partDao2->save();
        partDao3->save();
        partDao4->save();
    }

    auto convDaoA = make_unique<Dao::ConversationDao>(session_);
    auto convDaoB = make_unique<Dao::ConversationDao>(session_);

    convDaoA->setParticipants(std::set<Util::KulloAddress>{addrA_});
    convDaoB->setParticipants(std::set<Util::KulloAddress>{addrB_});

    convDaoA->save();
    convDaoB->save();

    auto msg1 = createMessage(msgId1_, addrB_.toString(), convDaoA->id());
    auto msg2 = createMessage(msgId2_, addrB_.toString(), convDaoB->id());
    auto msg3 = createMessage(msgId3_, addrB_.toString(), convDaoB->id());
    auto msg4 = createMessage(msgId4_, addrA_.toString(), convDaoA->id());
    K_UNUSED(msg1);
    K_UNUSED(msg2);
    K_UNUSED(msg3);
    K_UNUSED(msg4);

    Model::Conversation convA(client_.get(), convDaoA.release());
    Model::Conversation convB(client_.get(), convDaoB.release());
    convA.loadMessages();
    convB.loadMessages();

    EXPECT_LT(convB, convA);
}

TEST_F(ConversationModel, compareConversationsEqual)
{
    // msg1 < msg2 == msg3 < msg4
    // convA = (msg1, msg2)
    // convB = (msg3)
    // (msg1, msg2) == (msg3)
    // convA == convB

    {
        auto partDao1 = make_unique<Dao::ParticipantDao>(addrA_, session_);
        auto partDao2 = make_unique<Dao::ParticipantDao>(addrA_, session_);
        auto partDao3 = make_unique<Dao::ParticipantDao>(addrB_, session_);
        partDao1->setMessageId(msgId1_);
        partDao2->setMessageId(msgId2_);
        partDao3->setMessageId(msgId3_);
        partDao1->save();
        partDao2->save();
        partDao3->save();
    }

    auto convDaoA = make_unique<Dao::ConversationDao>(session_);
    auto convDaoB = make_unique<Dao::ConversationDao>(session_);

    convDaoA->setParticipants(std::set<Util::KulloAddress>{addrA_});
    convDaoB->setParticipants(std::set<Util::KulloAddress>{addrB_});

    convDaoA->save();
    convDaoB->save();

    auto msg1 = createMessage(msgId1_, addrA_.toString(), convDaoA->id());
    auto msg2 = createMessage(msgId2_, addrA_.toString(), convDaoA->id());
    auto msg3 = createMessage(msgId3_, addrB_.toString(), convDaoB->id());
    K_UNUSED(msg1);
    K_UNUSED(msg2);
    K_UNUSED(msg3);

    Model::Conversation convA(client_.get(), convDaoA.release());
    Model::Conversation convB(client_.get(), convDaoB.release());
    convA.loadMessages();
    convB.loadMessages();

    EXPECT_FALSE(convA < convB);
    EXPECT_FALSE(convA > convB);
}

TEST_F(ConversationModel, sortConversationsWidth2Messages)
{
    // msg1 < msg2 = msg3 < msg4
    // convA = (msg1, msg4)
    // convB = (msg2, msg3)
    // (msg2, msg3) < (msg1, msg4)
    // convB < convA

    {
        auto partDao1 = make_unique<Dao::ParticipantDao>(addrA_, session_);
        auto partDao2 = make_unique<Dao::ParticipantDao>(addrB_, session_);
        auto partDao3 = make_unique<Dao::ParticipantDao>(addrB_, session_);
        auto partDao4 = make_unique<Dao::ParticipantDao>(addrA_, session_);
        partDao1->setMessageId(msgId1_);
        partDao2->setMessageId(msgId2_);
        partDao3->setMessageId(msgId3_);
        partDao4->setMessageId(msgId4_);
        partDao1->save();
        partDao2->save();
        partDao3->save();
        partDao4->save();
    }

    auto convDaoA = make_unique<Dao::ConversationDao>(session_);
    auto convDaoB = make_unique<Dao::ConversationDao>(session_);

    convDaoA->setParticipants(std::set<Util::KulloAddress>{addrA_});
    convDaoB->setParticipants(std::set<Util::KulloAddress>{addrB_});

    convDaoA->save();
    convDaoB->save();

    auto msg1 = createMessage(msgId1_, addrA_.toString(), convDaoA->id());
    auto msg2 = createMessage(msgId2_, addrB_.toString(), convDaoB->id());
    auto msg3 = createMessage(msgId3_, addrB_.toString(), convDaoB->id());
    auto msg4 = createMessage(msgId4_, addrA_.toString(), convDaoA->id());
    K_UNUSED(msg1);
    K_UNUSED(msg2);
    K_UNUSED(msg3);
    K_UNUSED(msg4);

    Model::Conversation convA(client_.get(), convDaoA.release());
    Model::Conversation convB(client_.get(), convDaoB.release());
    convA.loadMessages();
    convB.loadMessages();

    // Create vecor: convA, convB
    auto conversationsList = std::vector<Model::Conversation*>{&convA, &convB};
    EXPECT_THAT(conversationsList[0]->id(), Eq(convA.id()));
    EXPECT_THAT(conversationsList[1]->id(), Eq(convB.id()));

    // convB < convA
    // Sort ascending: convB, convA
    std::sort(conversationsList.begin(), conversationsList.end(),
              [](Model::Conversation* lhs, Model::Conversation* rhs) { return *lhs < *rhs; }
    );
    EXPECT_THAT(conversationsList[0]->id(), Eq(convB.id()));
    EXPECT_THAT(conversationsList[1]->id(), Eq(convA.id()));

    // convB < convA
    // Sort decending: convA, convB
    std::sort(conversationsList.begin(), conversationsList.end(),
              [](Model::Conversation* lhs, Model::Conversation* rhs) { return *lhs > *rhs; }
    );
    EXPECT_THAT(conversationsList[0]->id(), Eq(convA.id()));
    EXPECT_THAT(conversationsList[1]->id(), Eq(convB.id()));
}

TEST_F(ConversationModel, messageCountsOneMessagePerConversation)
{
    {
        auto partDao1 = make_unique<Dao::ParticipantDao>(addrA_, session_);
        auto partDao2 = make_unique<Dao::ParticipantDao>(addrB_, session_);
        auto partDao3 = make_unique<Dao::ParticipantDao>(addrC_, session_);
        auto partDao4 = make_unique<Dao::ParticipantDao>(addrD_, session_);
        partDao1->setMessageId(msgId1_);
        partDao2->setMessageId(msgId2_);
        partDao3->setMessageId(msgId3_);
        partDao4->setMessageId(msgId4_);
        partDao1->save();
        partDao2->save();
        partDao3->save();
        partDao4->save();
    }

    auto convDao1 = make_unique<Dao::ConversationDao>(session_);
    auto convDao2 = make_unique<Dao::ConversationDao>(session_);
    auto convDao3 = make_unique<Dao::ConversationDao>(session_);
    auto convDao4 = make_unique<Dao::ConversationDao>(session_);

    convDao1->setParticipants(std::set<Util::KulloAddress>{addrA_});
    convDao2->setParticipants(std::set<Util::KulloAddress>{addrB_});
    convDao3->setParticipants(std::set<Util::KulloAddress>{addrC_});
    convDao4->setParticipants(std::set<Util::KulloAddress>{addrD_});

    convDao1->save();
    convDao2->save();
    convDao3->save();
    convDao4->save();

    auto msg1 = createMessage(msgId1_, addrA_.toString(), convDao1->id());
    auto msg2 = createMessage(msgId2_, addrB_.toString(), convDao2->id());
    auto msg3 = createMessage(msgId3_, addrC_.toString(), convDao3->id());
    auto msg4 = createMessage(msgId4_, addrD_.toString(), convDao4->id());
    K_UNUSED(msg1);
    K_UNUSED(msg2);
    K_UNUSED(msg3);
    K_UNUSED(msg4);

    Model::Conversation conv1(client_.get(), convDao1.release());
    Model::Conversation conv2(client_.get(), convDao2.release());
    Model::Conversation conv3(client_.get(), convDao3.release());
    Model::Conversation conv4(client_.get(), convDao4.release());
    conv1.loadMessages();
    conv2.loadMessages();
    conv3.loadMessages();
    conv4.loadMessages();

    EXPECT_THAT(conv1.countMessages(Dao::MessageState::Any),    Eq(size_t{1}));
    EXPECT_THAT(conv1.countMessages(Dao::MessageState::Read),   Eq(size_t{0}));
    EXPECT_THAT(conv1.countMessages(Dao::MessageState::Unread), Eq(size_t{1}));
    EXPECT_THAT(conv1.countMessages(Dao::MessageState::Done),   Eq(size_t{0}));
    EXPECT_THAT(conv1.countMessages(Dao::MessageState::Undone), Eq(size_t{1}));

    EXPECT_THAT(conv2.countMessages(Dao::MessageState::Any),    Eq(size_t{1}));
    EXPECT_THAT(conv2.countMessages(Dao::MessageState::Read),   Eq(size_t{1}));
    EXPECT_THAT(conv2.countMessages(Dao::MessageState::Unread), Eq(size_t{0}));
    EXPECT_THAT(conv2.countMessages(Dao::MessageState::Done),   Eq(size_t{0}));
    EXPECT_THAT(conv2.countMessages(Dao::MessageState::Undone), Eq(size_t{1}));

    EXPECT_THAT(conv3.countMessages(Dao::MessageState::Any),    Eq(size_t{1}));
    EXPECT_THAT(conv3.countMessages(Dao::MessageState::Read),   Eq(size_t{0}));
    EXPECT_THAT(conv3.countMessages(Dao::MessageState::Unread), Eq(size_t{1}));
    EXPECT_THAT(conv3.countMessages(Dao::MessageState::Done),   Eq(size_t{1}));
    EXPECT_THAT(conv3.countMessages(Dao::MessageState::Undone), Eq(size_t{0}));

    EXPECT_THAT(conv4.countMessages(Dao::MessageState::Any),    Eq(size_t{1}));
    EXPECT_THAT(conv4.countMessages(Dao::MessageState::Read),   Eq(size_t{1}));
    EXPECT_THAT(conv4.countMessages(Dao::MessageState::Unread), Eq(size_t{0}));
    EXPECT_THAT(conv4.countMessages(Dao::MessageState::Done),   Eq(size_t{1}));
    EXPECT_THAT(conv4.countMessages(Dao::MessageState::Undone), Eq(size_t{0}));
}

TEST_F(ConversationModel, messageCountsAllMessageInOneConversation)
{
    {
        auto partDao1 = make_unique<Dao::ParticipantDao>(addrA_, session_);
        auto partDao2 = make_unique<Dao::ParticipantDao>(addrB_, session_);
        auto partDao3 = make_unique<Dao::ParticipantDao>(addrC_, session_);
        auto partDao4 = make_unique<Dao::ParticipantDao>(addrD_, session_);
        partDao1->setMessageId(msgId1_);
        partDao2->setMessageId(msgId2_);
        partDao3->setMessageId(msgId3_);
        partDao4->setMessageId(msgId4_);
        partDao1->save();
        partDao2->save();
        partDao3->save();
        partDao4->save();
    }

    auto convDao = make_unique<Dao::ConversationDao>(session_);
    convDao->setParticipants(std::set<Util::KulloAddress>{addrA_, addrB_, addrC_, addrD_});
    convDao->save();

    auto msg1 = createMessage(msgId1_, addrA_.toString(), convDao->id());
    auto msg2 = createMessage(msgId2_, addrB_.toString(), convDao->id());
    auto msg3 = createMessage(msgId3_, addrC_.toString(), convDao->id());
    auto msg4 = createMessage(msgId4_, addrD_.toString(), convDao->id());
    K_UNUSED(msg1);
    K_UNUSED(msg2);
    K_UNUSED(msg3);
    K_UNUSED(msg4);

    Model::Conversation conv(client_.get(), convDao.release());
    conv.loadMessages();

    EXPECT_THAT(conv.countMessages(Dao::MessageState::Any),    Eq(size_t{4}));
    EXPECT_THAT(conv.countMessages(Dao::MessageState::Read),   Eq(size_t{2}));
    EXPECT_THAT(conv.countMessages(Dao::MessageState::Unread), Eq(size_t{2}));
    EXPECT_THAT(conv.countMessages(Dao::MessageState::Done),   Eq(size_t{2}));
    EXPECT_THAT(conv.countMessages(Dao::MessageState::Undone), Eq(size_t{2}));
}
