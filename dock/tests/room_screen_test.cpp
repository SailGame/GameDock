#include <sailgame_pb/core/types.pb.h>
#include <ftxui/screen/string.hpp>
#include <google/protobuf/util/message_differencer.h>

#include "matcher.h"
#include "screen_fixture.h"

namespace SailGame { namespace Test {

using namespace testing;
using namespace Dock;
using Common::CoreMsgBuilder;
using ::Core::ErrorNumber;
using ::Core::Ready;
using ftxui::to_wstring;
using grpc::Status;
using google::protobuf::util::MessageDifferencer;

class RoomScreenFixture : public ScreenFixture {
public:
    RoomScreenFixture() : ScreenFixture() {
        LoginSuccess(mToken);
        mDock.mRoomScreen.TakeFocus();
    }

protected:
    std::string mToken{"tok"};
};

TEST_F(RoomScreenFixture, ToggleReady) {
    EXPECT_FALSE(mDock.mRoomScreen.mIsReady);

    Ready ready = Ready::READY;
    auto readyRet = CoreMsgBuilder::CreateOperationInRoomRet(ErrorNumber::OK);
    EXPECT_CALL(*mMockStub, OperationInRoom(_, ReadyMatcher(mToken, ready), _))
        .Times(1)
        .WillOnce(DoAll(SetArgPointee<2>(readyRet), Return(Status::OK)));
    UserEvent(mDock.mRoomScreen.mReadyToggleButton.on_click);
    EXPECT_TRUE(mDock.mRoomScreen.mIsReady);

    ready = Ready::CANCEL;
    EXPECT_CALL(*mMockStub, OperationInRoom(_, ReadyMatcher(mToken, ready), _))
        .Times(1)
        .WillOnce(DoAll(SetArgPointee<2>(readyRet), Return(Status::OK)));
    UserEvent(mDock.mRoomScreen.mReadyToggleButton.on_click);
    EXPECT_FALSE(mDock.mRoomScreen.mIsReady);
}

TEST_F(RoomScreenFixture, NewPlayerJoins) {
    RoomDetails roomDetails = CoreMsgBuilder::CreateRoomDetails("UNO", 101, {
        CoreMsgBuilder::CreateRoomUser("a", RoomUser::READY),
        CoreMsgBuilder::CreateRoomUser("b", RoomUser::PREPARING),
    }, Uno::MsgBuilder::CreateStartGameSettings(true, true, false, false, 15));
    CoreMsg(
        CoreMsgBuilder::CreateBroadcastMsgByRoomDetails(0, 0, 0, roomDetails));
    EXPECT_TRUE(MessageDifferencer::Equals(
        mDock.mRoomScreen.GetState().mRoomDetails, roomDetails));
    UserEvent();
    
    // now comes a new player
    roomDetails.add_user()->CopyFrom(CoreMsgBuilder::CreateRoomUser(
        "new-joiner", RoomUser::PREPARING));
    CoreMsg(
        CoreMsgBuilder::CreateBroadcastMsgByRoomDetails(0, 0, 0, roomDetails));
    EXPECT_TRUE(MessageDifferencer::Equals(
        mDock.mRoomScreen.GetState().mRoomDetails, roomDetails));
    UserEvent();
}
}}