#include <google/protobuf/util/message_differencer.h>
#include <sailgame_pb/core/types.pb.h>

#include <ftxui/screen/string.hpp>

#include "../matcher.h"
#include "../screen_fixture.h"

namespace SailGame { namespace Test {

using namespace testing;
using namespace Dock;
using Common::CoreMsgBuilder;
using Common::GameType;
using ::Core::ErrorNumber;
using ::Core::Ready;
using ftxui::to_wstring;
using google::protobuf::util::MessageDifferencer;
using grpc::Status;

class RoomScreenFixture : public ScreenFixture {
public:
    RoomScreenFixture() : ScreenFixture() {
        LoginSuccess(mToken);
        mDock.mRoomScreen->TakeFocus();
    }

protected:
    std::string mToken{"tok"};
};

TEST_F(RoomScreenFixture, ToggleReady) {
    EXPECT_FALSE(mDock.mRoomScreen->mIsReady);

    Ready ready = Ready::READY;
    auto readyRet = CoreMsgBuilder::CreateOperationInRoomRet(ErrorNumber::OK);
    EXPECT_CALL(*mMockStub, OperationInRoom(_, ReadyMatcher(mToken, ready), _))
        .Times(1)
        .WillOnce(DoAll(SetArgPointee<2>(readyRet), Return(Status::OK)));
    mDock.mRoomScreen->mReadyToggleButton->OnEvent(Event::Return);
    EXPECT_TRUE(mDock.mRoomScreen->mIsReady);

    ready = Ready::CANCEL;
    EXPECT_CALL(*mMockStub, OperationInRoom(_, ReadyMatcher(mToken, ready), _))
        .Times(1)
        .WillOnce(DoAll(SetArgPointee<2>(readyRet), Return(Status::OK)));
    mDock.mRoomScreen->mReadyToggleButton->OnEvent(Event::Return);
    EXPECT_FALSE(mDock.mRoomScreen->mIsReady);
}

TEST_F(RoomScreenFixture, ExitRoom) {
    auto exitRoomRet = CoreMsgBuilder::CreateExitRoomRet(ErrorNumber::OK);
    EXPECT_CALL(*mMockStub, ExitRoom(_, ExitRoomArgsMatcher(mToken), _))
        .Times(1)
        .WillOnce(DoAll(SetArgPointee<2>(exitRoomRet), Return(Status::OK)));
    mDock.mRoomScreen->mExitRoomButton->OnEvent(Event::Return);
    EXPECT_TRUE(mDock.mLobbyScreen->Focused());
}

TEST_F(RoomScreenFixture, NewPlayerJoins) {
    RoomDetails roomDetails = CoreMsgBuilder::CreateRoomDetails(
        "UNO", 101,
        {
            CoreMsgBuilder::CreateRoomUser("a", RoomUser::READY),
            CoreMsgBuilder::CreateRoomUser("b", RoomUser::PREPARING),
        },
        Uno::MsgBuilder::CreateStartGameSettings(true, true, false, false, 15));
    CoreMsg(
        CoreMsgBuilder::CreateBroadcastMsgByRoomDetails(0, 0, 0, roomDetails));
    EXPECT_TRUE(MessageDifferencer::Equals(
        mDock.mRoomScreen->GetState().mRoomDetails, roomDetails));

    // now comes a new player
    roomDetails.add_user()->CopyFrom(
        CoreMsgBuilder::CreateRoomUser("new-joiner", RoomUser::PREPARING));
    CoreMsg(
        CoreMsgBuilder::CreateBroadcastMsgByRoomDetails(0, 0, 0, roomDetails));
    EXPECT_TRUE(MessageDifferencer::Equals(
        mDock.mRoomScreen->GetState().mRoomDetails, roomDetails));
}

TEST_F(RoomScreenFixture, GameStart) {
    mDock.mRoomScreen->mGameList->TakeFocus();
    mDock.mRoomScreen->mGameList->OnEvent(Event::ArrowDown);

    auto controlRoomRet = CoreMsgBuilder::CreateControlRoomRet(
        ErrorNumber::OK, google::protobuf::Any());
    EXPECT_CALL(*mMockStub, ControlRoom(_, _, _))
        .Times(1)
        .WillOnce(DoAll(SetArgPointee<2>(controlRoomRet), Return(Status::OK)));
    mDock.mRoomScreen->mGameList->OnEvent(Event::Return);

    RoomDetails roomDetails = CoreMsgBuilder::CreateRoomDetails(
        "UNO", 102,
        {
            CoreMsgBuilder::CreateRoomUser("a", RoomUser::READY),
            CoreMsgBuilder::CreateRoomUser("b", RoomUser::PREPARING),
            CoreMsgBuilder::CreateRoomUser("c", RoomUser::READY),
            // by default, `my` username is `test`
            CoreMsgBuilder::CreateRoomUser("test", RoomUser::READY),
        },
        Uno::MsgBuilder::CreateStartGameSettings(true, true, false, false, 15));
    CoreMsg(
        CoreMsgBuilder::CreateBroadcastMsgByRoomDetails(0, 0, 0, roomDetails));

    // b gets ready, game start now
    // note that when all users are ready, Core will automatically
    // change user state to PLAYING
    for (auto i = 0; i < 4; i++) {
        roomDetails.mutable_user(i)->set_userstate(RoomUser::PLAYING);
    }
    GameStart(roomDetails);
}

}}  // namespace SailGame::Test