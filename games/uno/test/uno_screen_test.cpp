#include <sailgame_pb/uno/uno.pb.h>
#include <sailgame/common/core_msg_builder.h>
#include <sailgame/uno/msg_builder.h>

#include "../../../dock/tests/screen_fixture.h"

namespace SailGame { namespace Test {

using namespace testing;
using Common::CoreMsgBuilder;
using Uno::InitHandcardsT;
using Uno::MsgBuilder;

class UnoScreenFixture : public ScreenFixture {
public:
    UnoScreenFixture() : ScreenFixture() {
        LoginSuccess("toki", mUsername);
        mDock.mRoomScreen.TakeFocus();
    }

    void SetUp() override {
        ScreenFixture::SetUp();
        RoomDetails roomDetails = CoreMsgBuilder::CreateRoomDetails(
            "UNO", 102,
            {
                CoreMsgBuilder::CreateRoomUser("A", RoomUser::READY),
                CoreMsgBuilder::CreateRoomUser(mUsername, RoomUser::READY),
                CoreMsgBuilder::CreateRoomUser("B", RoomUser::READY),
                CoreMsgBuilder::CreateRoomUser("C", RoomUser::READY),
            },
            Uno::MsgBuilder::CreateStartGameSettings(
                true, true, false, false, 15));
        GameStart(roomDetails);
    }

    void GetGameStartMsg(const ::Uno::NotifyMsg &gameStart) {
        CoreMsg(CoreMsgBuilder::CreateBroadcastMsgByNotifyMsg(
            0, 0, 1, gameStart));
        UserEvent();
    }

protected:
    std::string mUsername{"me"};
};

TEST_F(UnoScreenFixture, NotMyTurn) { 
    InitHandcardsT initHandcards = {"R1", "Y+2", "GR", "BS", "W", "+4", "Y0"};
    GetGameStartMsg(MsgBuilder::CreateGameStart(initHandcards, "Y2", 0));
    auto state = std::dynamic_pointer_cast<Uno::GameScreen>(
        mDock.mGameScreen.mGameScreen)->GetState();
    EXPECT_EQ(state.mGameState.mSelfPlayerIndex, 1);
    EXPECT_EQ(state.mGameState.mLastPlayedCard, "Y2");
    EXPECT_EQ(state.mGameState.mCurrentPlayer, 0);
}
}}