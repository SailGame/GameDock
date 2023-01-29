#pragma once

#include <grpcpp/test/mock_stream.h>
#include <gtest/gtest.h>
#include <sailgame_pb/core/core_mock.grpc.pb.h>
#include <sailgame_pb/core/types.pb.h>

#include <ftxui/screen/string.hpp>

#include "../core/game_attr_fac.h"
#include "../dock.h"
#include "../util/util.hpp"
#include "matcher.h"

namespace SailGame { namespace Test {

using namespace testing;
using namespace Dock;
using Common::ClientNetworkInterface;
using Common::CoreMsgBuilder;
using Common::Util;
using ::Core::BroadcastMsg;
using ::Core::ErrorNumber;
using ::Core::MockGameCoreStub;
using ::Core::RoomDetails;
using ftxui::to_string;
using ftxui::to_wstring;
using grpc::Status;
using grpc::testing::MockClientReader;

class ScreenFixture : public ::testing::Test {
public:
    ScreenFixture()
        : mMockStream(new MockClientReader<BroadcastMsg>()),
          mMockStub(std::make_shared<MockGameCoreStub>()),
          mDock(UIProxy::Create(ClientNetworkInterface::Create(mMockStub),
                                true)) {}

    void SetUp() {}

    void TearDown() { mDock.mUIProxy->Stop(); }

    void LoginSuccess(const std::string &token,
                      const std::string &username = "test") {
        EXPECT_CALL(*mMockStub, ListenRaw(_, ListenArgsMatcher(token)))
            .Times(1)
            .WillOnce(Return(mMockStream));
        mDock.mUIProxy->OnLoginSuccess(token, username);

        while (!mDock.mUIProxy->mNetworkInterface->IsConnected()) {
        }
    }

    void GameStart(const RoomDetails &roomDetails) {
        mDock.mRoomScreen->TakeFocus();
        ASSERT_EQ(mDock.mUIProxy->mGameManager->GetGameType(),
                  GameType::NoGame);

        CoreMsg(CoreMsgBuilder::CreateBroadcastMsgByRoomDetails(0, 0, 0,
                                                                roomDetails));
        // next frame: all players are ready in room screen
        ASSERT_TRUE(mDock.mPolyGameScreen->HasComponent());
        EXPECT_TRUE(mDock.mPolyGameScreen->Active());
        auto gameType = Util::GetGameTypeByGameName(roomDetails.gamename());
        EXPECT_EQ(mDock.mUIProxy->mGameManager->GetGameType(), gameType);
        EXPECT_EQ(mDock.mPolyGameScreen->GetComponent()->GetGameType(),
                  gameType);
    }

    void CoreMsg(const BroadcastMsg &msg) {
        EXPECT_CALL(*mMockStream, Read(_))
            .Times(1)
            .WillOnce(DoAll(SetArgPointee<0>(msg), Return(true)));
        auto &networkInterface = mDock.mUIProxy->mNetworkInterface;
        networkInterface->OnEventHappens(networkInterface->ReceiveMsg());

        while (mDock.mUIProxy->mGameManager->HasEventToProcess()) {
        }
    }

protected:
    MockClientReader<BroadcastMsg> *mMockStream;
    std::shared_ptr<MockGameCoreStub> mMockStub;

    SailGame::Dock::Dock mDock;
};

}}  // namespace SailGame::Test
