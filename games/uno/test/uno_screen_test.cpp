#include <sailgame_pb/uno/uno.pb.h>
#include <sailgame/common/core_msg_builder.h>
#include <sailgame/uno/msg_builder.h>

#include "../../../dock/tests/screen_fixture.h"
#include "../src/state.h"

namespace SailGame { namespace Test {

using namespace testing;
using Common::CoreMsgBuilder;
using ::Uno::CardColor;
using Uno::InitHandcardsT;
using Uno::MsgBuilder;
using Uno::WholeState;

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
        // what is behind UserEvent could also be rendered in that frame
        // because UserEvent just posts a Custom Event, which doesn't means
        // this event will be immediately consumed in ftxui thread
        // NOTE: this should be fixed now by sleep 0.5s both before and
        // after posting Custom Event.
        CoreMsg(CoreMsgBuilder::CreateBroadcastMsgByNotifyMsg(
            0, 0, 1, gameStart));
        UserEvent();
    }

    WholeState GetState() {
        return GetScreen()->GetState();
    }

    std::shared_ptr<Uno::GameScreen> GetScreen() {
        return std::dynamic_pointer_cast<Uno::GameScreen>(
            mDock.mGameScreen.mGameScreen);
    }

protected:
    std::string mUsername{"me"};
};

TEST_F(UnoScreenFixture, NotMyTurn) { 
    InitHandcardsT initHandcards = {"R1", "Y+2", "GR", "BS", "W", "+4", "Y0"};
    GetGameStartMsg(MsgBuilder::CreateGameStart(initHandcards, "Y2", 0));
    EXPECT_EQ(GetState().mGameState.mSelfPlayerIndex, 1);
    EXPECT_EQ(GetState().mGameState.mLastPlayedCard, "Y2");
    EXPECT_EQ(GetState().mGameState.mCurrentPlayer, 0);
    EXPECT_TRUE(GetScreen()->mNotMyTurnPanel.Focused());
}

TEST_F(UnoScreenFixture, OnMyTurn) {
    InitHandcardsT initHandcards = {"R1", "Y+2", "GR", "BS", "W", "+4", "Y0"};
    GetGameStartMsg(MsgBuilder::CreateGameStart(initHandcards, "Y2", 0));

    CoreMsg(CoreMsgBuilder::CreateBroadcastMsgByNotifyMsg(
        0, 0, 0, MsgBuilder::CreateDraw(1)));
    UserEvent();
    EXPECT_EQ(GetState().mPlayerStates[0].mRemainingHandCardsNum, 8);
    EXPECT_FALSE(GetState().mPlayerStates[0].mDoPlayInLastRound);
    EXPECT_EQ(GetState().mGameState.mLastPlayedCard, "Y2");
    EXPECT_EQ(GetState().mGameState.mCardsNumToDraw, 1);

    CoreMsg(CoreMsgBuilder::CreateBroadcastMsgByNotifyMsg(
        0, 0, 0, MsgBuilder::CreateSkip()));
    UserEvent();
    EXPECT_FALSE(GetState().mPlayerStates[0].mDoPlayInLastRound);
    // player order: 0 -> 1 -> 2 -> 3
    EXPECT_TRUE(GetState().mGameState.IsMyTurn());

    UserEvent();
    EXPECT_TRUE(GetScreen()->mPlayOrPassPanel.Focused());
}

TEST_F(UnoScreenFixture, PlayCard) {
    InitHandcardsT initHandcards = {"R1", "Y+2", "GR", "BS", "W", "+4", "+4"};
    GetGameStartMsg(MsgBuilder::CreateGameStart(initHandcards, "G2", 1));
    UserEvent();
    EXPECT_TRUE(GetScreen()->mPlayOrPassPanel.Focused());

    UserEvent(GetScreen()->mPlayOrPassPanel.mPlayButton.on_click);
    EXPECT_TRUE(GetScreen()->mChooseCardPanel.Focused());
    EXPECT_EQ(GetScreen()->mChooseCardPanel.mCursor, 0);

    UserEvent(GetScreen()->mChooseCardPanel.mHandcardsSelector.OnMoveRight);
    EXPECT_EQ(GetScreen()->mChooseCardPanel.mCursor, 1);

    /// TODO: add test case for illegal play

    UserEvent(GetScreen()->mChooseCardPanel.mHandcardsSelector.OnMoveRight);
    EXPECT_EQ(GetScreen()->mChooseCardPanel.mCursor, 2);

    auto opRet = CoreMsgBuilder::CreateOperationInRoomRet(ErrorNumber::OK);
    EXPECT_CALL(*mMockStub, OperationInRoom(_, _, _))
        .Times(1)
        .WillOnce(DoAll(SetArgPointee<2>(opRet), Return(Status::OK)));
    UserEvent(GetScreen()->mChooseCardPanel.mHandcardsSelector.OnPlay);
    CoreMsg(CoreMsgBuilder::CreateBroadcastMsgByNotifyMsg(
        0, 0, 0, MsgBuilder::CreatePlay("GR", CardColor::GREEN)));
    UserEvent();
    EXPECT_EQ(GetState().mSelfState.mHandcards.Number(), 6);
    EXPECT_EQ(GetState().mPlayerStates[1].mRemainingHandCardsNum, 6);
    EXPECT_TRUE(GetState().mPlayerStates[1].mDoPlayInLastRound);
    EXPECT_EQ(GetState().mPlayerStates[1].mLastPlayedCard, "GR");
    EXPECT_EQ(GetState().mGameState.mLastPlayedCard, "GR");
    EXPECT_FALSE(GetState().mGameState.mIsInClockwise);
    EXPECT_EQ(GetState().mGameState.mCardsNumToDraw, 1);
    EXPECT_EQ(GetState().mGameState.mCurrentPlayer, 0);

    EXPECT_TRUE(GetScreen()->mNotMyTurnPanel.Focused());
    UserEvent();
}

TEST_F(UnoScreenFixture, Skip) {

}

TEST_F(UnoScreenFixture, DrawAndPlay) {
}

TEST_F(UnoScreenFixture, DrawAndNoPlay) {
}

}}