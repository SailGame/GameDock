#pragma once

#include <gtest/gtest.h>
#include <grpcpp/test/mock_stream.h>

#include <sailgame_pb/core/types.pb.h>
#include <sailgame_pb/core/core_mock.grpc.pb.h>
#include <ftxui/screen/string.hpp>

#include "../dock.h"
#include "matcher.h"
#include "../util/util.hpp"

namespace SailGame { namespace Test {

using namespace testing;
using namespace Dock;
using Common::CoreMsgBuilder;
using Common::NetworkInterface;
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
        mDock(UIProxy::Create(
            NetworkInterface<false>::Create(mMockStub), true))
    {}

    void SetUp() {
        spdlog::set_level(spdlog::level::err);
        // mTimerThread = std::make_unique<std::thread>([this] {
        //     for (;;) {
        //         using namespace std::chrono_literals;
        //         std::this_thread::sleep_for(0.05s);
        //         mDock.mScreen.PostEvent(ftxui::Event::Custom);
        //     }
        // });
        mDockThread = std::make_unique<std::thread>([this] {
            mDock.Loop();
        });
    }

    void TearDown() {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1s);
        mDock.mScreen.ExitLoopClosure()();
        mDock.mUIProxy->Stop();
        mDockThread->join();
    }

    void LoginSuccess(const std::string &token, 
        const std::string &username = "test") 
    {
        EXPECT_CALL(*mMockStub, ListenRaw(_, ListenArgsMatcher(token)))
            .Times(1)
            .WillOnce(Return(mMockStream));
        mDock.mUIProxy->OnLoginSuccess(token, username);
    }

    void GameStart(const RoomDetails &roomDetails) {
        mDock.mRoomScreen.TakeFocus();
        CoreMsg(CoreMsgBuilder::CreateBroadcastMsgByRoomDetails(
            0, 0, 0, roomDetails));
        EXPECT_TRUE(mDock.mRoomScreen.AreAllUsersReady());
        EXPECT_EQ(mDock.mUIProxy->mGameManager->GetGameType(), GameType::NoGame);
        EXPECT_EQ(mDock.mGameScreen.GetGameType(), GameType::NoGame);

        // next frame: all players are ready in room screen
        UserEvent();
        EXPECT_TRUE(mDock.mGameScreen.Focused());
        auto gameType = DockUtil::GetGameTypeByGameName(roomDetails.gamename());
        EXPECT_EQ(mDock.mUIProxy->mGameManager->GetGameType(), gameType);
        EXPECT_EQ(mDock.mGameScreen.GetGameType(), gameType);

        // next frame: game screen
        UserEvent();
    }

    void UserEvent(const std::function<void()> &callback = []{}) {
        // wait for 1 seconds to display ui
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(0.5s);
        callback();
        // post an event to refresh ui
        mDock.mScreen.PostEvent(ftxui::Event::Custom);
        std::this_thread::sleep_for(0.5s);
    }

    void CoreMsg(const BroadcastMsg &msg) {
        EXPECT_CALL(*mMockStream, Read(_)).Times(1).WillOnce(
            DoAll(SetArgPointee<0>(msg), Return(true)));
        auto &networkInterface = mDock.mUIProxy->mNetworkInterface;
        networkInterface->OnEventHappens(networkInterface->ReceiveMsg());

        while (mDock.mUIProxy->mGameManager->HasEventToProcess()) {}
    }

    void Refresh() {
        using namespace std::chrono_literals;
        // update focus in Render()
        mDock.mScreen.PostEvent(ftxui::Event::Custom);
        std::this_thread::sleep_for(0.5s);
        // re-Render to show the update
        mDock.mScreen.PostEvent(ftxui::Event::Custom);
        std::this_thread::sleep_for(0.5s);
    }

protected:
    MockClientReader<BroadcastMsg> *mMockStream;
    std::shared_ptr<MockGameCoreStub> mMockStub;

    Dock::Dock mDock;
    std::unique_ptr<std::thread> mDockThread;
};

}}
