#include <sailgame/common/core_msg_builder.h>
#include <sailgame/uno/msg_builder.h>
#include <sailgame_pb/uno/uno.pb.h>

#include "../../../dock/tests/screen_fixture.h"
#include "../core/state.h"
#include "matcher.h"

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
                CoreMsgBuilder::CreateRoomUser("A", RoomUser::PLAYING),
                CoreMsgBuilder::CreateRoomUser(mUsername, RoomUser::PLAYING),
                CoreMsgBuilder::CreateRoomUser("B", RoomUser::PLAYING),
                CoreMsgBuilder::CreateRoomUser("C", RoomUser::PLAYING),
            },
            Uno::MsgBuilder::CreateStartGameSettings(true, true, false, false,
                                                     15));
        GameStart(roomDetails);
    }

    void GetGameStartMsg(const ::Uno::NotifyMsg &gameStart) {
        // what is behind UserEvent could also be rendered in that frame
        // because UserEvent just posts a Custom Event, which doesn't means
        // this event will be immediately consumed in ftxui thread
        /// NOTE: this should be fixed now by sleep 0.5s both before and
        /// after posting Custom Event.
        CoreMsg(
            CoreMsgBuilder::CreateBroadcastMsgByNotifyMsg(0, 0, 1, gameStart));
        UserEvent();
    }

    WholeState GetState() { return GetScreen()->GetState(); }

    std::shared_ptr<Uno::GameScreen> GetScreen() {
        return std::dynamic_pointer_cast<Uno::GameScreen>(
            mDock.mPolyGameScreen.GetComponent());
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
    EXPECT_EQ(GetState().mPlayerStates[0].mRemainingHandCardsNum, 8);
    EXPECT_FALSE(GetState().mPlayerStates[0].mDoPlayInLastRound);
    EXPECT_EQ(GetState().mGameState.mLastPlayedCard, "Y2");
    EXPECT_EQ(GetState().mGameState.mCardsNumToDraw, 1);
    UserEvent();

    CoreMsg(CoreMsgBuilder::CreateBroadcastMsgByNotifyMsg(
        0, 0, 0, MsgBuilder::CreateSkip()));
    EXPECT_FALSE(GetState().mPlayerStates[0].mDoPlayInLastRound);
    // player order: 0 -> 1 -> 2 -> 3
    EXPECT_TRUE(GetState().mGameState.IsMyTurn());
    UserEvent();

    EXPECT_TRUE(GetScreen()->mPlayOrPassPanel.Focused());
    UserEvent();
}

TEST_F(UnoScreenFixture, PlayCard) {
    InitHandcardsT initHandcards = {"R1", "Y+2", "GR", "BS", "W", "+4", "+4"};
    GetGameStartMsg(MsgBuilder::CreateGameStart(initHandcards, "G2", 1));
    // the first frame is always NotMyTurnPanel, so an extra UserEvent
    // is needed to invoke NotMyTurnPanel's Render to re-render
    EXPECT_TRUE(GetScreen()->mPlayOrPassPanel.Focused());
    UserEvent();

    UserEvent(GetScreen()->mPlayOrPassPanel.mPlayButton.on_click);
    EXPECT_TRUE(GetScreen()->mChooseCardPanel.Focused());
    EXPECT_EQ(GetScreen()->mChooseCardPanel.mCursor, 0);

    UserEvent(GetScreen()->mChooseCardPanel.mHandcardsSelector.OnMoveRight);
    EXPECT_EQ(GetScreen()->mChooseCardPanel.mCursor, 1);

    UserEvent(GetScreen()->mChooseCardPanel.mHandcardsSelector.OnPlay);
    EXPECT_FALSE(GetScreen()->mChooseCardPanel.mHintText.empty());

    UserEvent(GetScreen()->mChooseCardPanel.mHandcardsSelector.OnMoveRight);
    EXPECT_EQ(GetScreen()->mChooseCardPanel.mCursor, 2);

    auto opRet = CoreMsgBuilder::CreateOperationInRoomRet(ErrorNumber::OK);
    EXPECT_CALL(*mMockStub,
                OperationInRoom(_, PlayMatcher("GR", CardColor::GREEN), _))
        .Times(1)
        .WillOnce(DoAll(SetArgPointee<2>(opRet), Return(Status::OK)));
    UserEvent(GetScreen()->mChooseCardPanel.mHandcardsSelector.OnPlay);

    CoreMsg(CoreMsgBuilder::CreateBroadcastMsgByNotifyMsg(
        0, 0, 0, MsgBuilder::CreatePlay("GR", CardColor::GREEN)));
    EXPECT_EQ(GetState().mSelfState.mHandcards.Number(), 6);
    EXPECT_EQ(GetState().mPlayerStates[1].mRemainingHandCardsNum, 6);
    EXPECT_TRUE(GetState().mPlayerStates[1].mDoPlayInLastRound);
    EXPECT_EQ(GetState().mPlayerStates[1].mLastPlayedCard, "GR");
    EXPECT_EQ(GetState().mGameState.mLastPlayedCard, "GR");
    EXPECT_FALSE(GetState().mGameState.mIsInClockwise);
    EXPECT_EQ(GetState().mGameState.mCardsNumToDraw, 1);
    EXPECT_EQ(GetState().mGameState.mCurrentPlayer, 0);

    UserEvent();
    EXPECT_TRUE(GetScreen()->mNotMyTurnPanel.Focused());
}

TEST_F(UnoScreenFixture, Skip) {
    InitHandcardsT initHandcards = {"R1", "Y+2", "GR", "B0", "W", "+4", "+4"};
    GetGameStartMsg(MsgBuilder::CreateGameStart(initHandcards, "BS", 1));
    EXPECT_TRUE(GetScreen()->mPlayOrPassPanel.Focused());
    UserEvent();

    auto opRet = CoreMsgBuilder::CreateOperationInRoomRet(ErrorNumber::OK);
    EXPECT_CALL(*mMockStub, OperationInRoom(_, SkipMatcher(), _))
        .Times(1)
        .WillOnce(DoAll(SetArgPointee<2>(opRet), Return(Status::OK)));
    UserEvent(GetScreen()->mPlayOrPassPanel.mPassButton.on_click);

    CoreMsg(CoreMsgBuilder::CreateBroadcastMsgByNotifyMsg(
        0, 0, 0, MsgBuilder::CreateSkip()));
    EXPECT_EQ(GetState().mSelfState.mHandcards.Number(), 7);
    EXPECT_FALSE(GetState().mPlayerStates[1].mDoPlayInLastRound);
    EXPECT_EQ(GetState().mGameState.mLastPlayedCard, "B");
    EXPECT_TRUE(GetState().mGameState.mIsInClockwise);
    EXPECT_EQ(GetState().mGameState.mCurrentPlayer, 2);

    UserEvent();
    EXPECT_TRUE(GetScreen()->mNotMyTurnPanel.Focused());
}

TEST_F(UnoScreenFixture, DrawAndPlay) {
    InitHandcardsT initHandcards = {"Y1", "Y+2", "GR", "B0", "B2", "+4", "+4"};
    GetGameStartMsg(MsgBuilder::CreateGameStart(initHandcards, "R4", 1));
    EXPECT_TRUE(GetScreen()->mPlayOrPassPanel.Focused());
    UserEvent();

    auto opRet = CoreMsgBuilder::CreateOperationInRoomRet(ErrorNumber::OK);
    EXPECT_CALL(*mMockStub, OperationInRoom(_, DrawMatcher(1), _))
        .Times(1)
        .WillOnce(DoAll(SetArgPointee<2>(opRet), Return(Status::OK)));
    UserEvent(GetScreen()->mPlayOrPassPanel.mPassButton.on_click);

    CoreMsg(CoreMsgBuilder::CreateBroadcastMsgByNotifyMsg(
        0, 0, 0, MsgBuilder::CreateDraw(1)));
    CoreMsg(CoreMsgBuilder::CreateBroadcastMsgByNotifyMsg(
        0, 0, 1, MsgBuilder::CreateDrawRsp({"R5"})));
    EXPECT_EQ(GetState().mSelfState.mHandcards.Number(), 8);
    EXPECT_EQ(GetState().mSelfState.mIndexOfNewlyDrawn, 0);
    EXPECT_TRUE(GetState().mSelfState.mHasChanceToPlayAfterDraw);
    EXPECT_TRUE(GetState().mGameState.IsMyTurn());

    UserEvent();
    EXPECT_TRUE(GetScreen()->mPlayImmediatelyPanel.Focused());
    UserEvent();

    EXPECT_CALL(*mMockStub,
                OperationInRoom(_, PlayMatcher("R5", CardColor::RED), _))
        .Times(1)
        .WillOnce(DoAll(SetArgPointee<2>(opRet), Return(Status::OK)));
    UserEvent(GetScreen()->mPlayImmediatelyPanel.mYesButton.on_click);

    CoreMsg(CoreMsgBuilder::CreateBroadcastMsgByNotifyMsg(
        0, 0, 0, MsgBuilder::CreatePlay("R5", CardColor::RED)));
    EXPECT_EQ(GetState().mSelfState.mHandcards.Number(), 7);
    EXPECT_TRUE(GetState().mPlayerStates[1].mDoPlayInLastRound);
    EXPECT_EQ(GetState().mGameState.mLastPlayedCard, "R5");
    EXPECT_TRUE(GetState().mGameState.mIsInClockwise);
    EXPECT_EQ(GetState().mGameState.mCurrentPlayer, 2);

    UserEvent();
    EXPECT_TRUE(GetScreen()->mNotMyTurnPanel.Focused());
}

TEST_F(UnoScreenFixture, DrawAndNoPlay) {
    InitHandcardsT initHandcards = {"Y1", "Y+2", "GR", "B0", "B2", "+4", "+4"};
    GetGameStartMsg(MsgBuilder::CreateGameStart(initHandcards, "R4", 1));
    EXPECT_TRUE(GetScreen()->mPlayOrPassPanel.Focused());
    UserEvent();

    auto opRet = CoreMsgBuilder::CreateOperationInRoomRet(ErrorNumber::OK);
    EXPECT_CALL(*mMockStub, OperationInRoom(_, DrawMatcher(1), _))
        .Times(1)
        .WillOnce(DoAll(SetArgPointee<2>(opRet), Return(Status::OK)));
    UserEvent(GetScreen()->mPlayOrPassPanel.mPassButton.on_click);

    CoreMsg(CoreMsgBuilder::CreateBroadcastMsgByNotifyMsg(
        0, 0, 0, MsgBuilder::CreateDraw(1)));
    CoreMsg(CoreMsgBuilder::CreateBroadcastMsgByNotifyMsg(
        0, 0, 1, MsgBuilder::CreateDrawRsp({"B5"})));
    EXPECT_EQ(GetState().mSelfState.mHandcards.Number(), 8);
    EXPECT_EQ(GetState().mSelfState.mIndexOfNewlyDrawn, 5);
    EXPECT_TRUE(GetState().mSelfState.mHasChanceToPlayAfterDraw);
    EXPECT_TRUE(GetState().mGameState.IsMyTurn());

    UserEvent();
    EXPECT_TRUE(GetScreen()->mPlayImmediatelyPanel.Focused());
    UserEvent();

    EXPECT_CALL(*mMockStub, OperationInRoom(_, SkipMatcher(), _))
        .Times(1)
        .WillOnce(DoAll(SetArgPointee<2>(opRet), Return(Status::OK)));
    UserEvent(GetScreen()->mPlayImmediatelyPanel.mNoButton.on_click);

    CoreMsg(CoreMsgBuilder::CreateBroadcastMsgByNotifyMsg(
        0, 0, 0, MsgBuilder::CreateSkip()));
    EXPECT_EQ(GetState().mSelfState.mHandcards.Number(), 8);
    EXPECT_FALSE(GetState().mPlayerStates[1].mDoPlayInLastRound);
    EXPECT_EQ(GetState().mGameState.mLastPlayedCard, "R4");
    EXPECT_TRUE(GetState().mGameState.mIsInClockwise);
    EXPECT_EQ(GetState().mGameState.mCurrentPlayer, 2);

    UserEvent();
    EXPECT_TRUE(GetScreen()->mNotMyTurnPanel.Focused());
}

TEST_F(UnoScreenFixture, SpecifyNextColorFromChooseCard) {
    InitHandcardsT initHandcards = {"Y1", "Y+2", "GR", "B0", "B2", "W", "+4"};
    GetGameStartMsg(MsgBuilder::CreateGameStart(initHandcards, "R4", 1));
    EXPECT_TRUE(GetScreen()->mPlayOrPassPanel.Focused());
    UserEvent();

    UserEvent(GetScreen()->mPlayOrPassPanel.mPlayButton.on_click);
    EXPECT_TRUE(GetScreen()->mChooseCardPanel.Focused());
    EXPECT_EQ(GetScreen()->mChooseCardPanel.mCursor, 0);

    UserEvent(GetScreen()->mChooseCardPanel.mHandcardsSelector.OnMoveLeft);
    EXPECT_EQ(GetScreen()->mChooseCardPanel.mCursor, 6);

    UserEvent(GetScreen()->mChooseCardPanel.mHandcardsSelector.OnMoveLeft);
    EXPECT_EQ(GetScreen()->mChooseCardPanel.mCursor, 5);

    UserEvent(GetScreen()->mChooseCardPanel.mHandcardsSelector.OnPlay);
    EXPECT_TRUE(GetScreen()->mSpecifyColorPanel.Focused());
    EXPECT_EQ(GetScreen()->mSpecifyColorPanel.mCursor, 5);
    UserEvent();

    // click cancel button will return to ChooseCardPanel
    UserEvent(GetScreen()->mSpecifyColorPanel.mCancelButton.on_click);
    EXPECT_TRUE(GetScreen()->mChooseCardPanel.Focused());
    UserEvent();

    UserEvent(GetScreen()->mChooseCardPanel.mHandcardsSelector.OnPlay);
    EXPECT_TRUE(GetScreen()->mSpecifyColorPanel.Focused());
    UserEvent();

    auto opRet = CoreMsgBuilder::CreateOperationInRoomRet(ErrorNumber::OK);
    EXPECT_CALL(*mMockStub,
                OperationInRoom(_, PlayMatcher("W", CardColor::YELLOW), _))
        .Times(1)
        .WillOnce(DoAll(SetArgPointee<2>(opRet), Return(Status::OK)));
    UserEvent(GetScreen()->mSpecifyColorPanel.mYellowButton.on_click);

    CoreMsg(CoreMsgBuilder::CreateBroadcastMsgByNotifyMsg(
        0, 0, 0, MsgBuilder::CreatePlay("W", CardColor::YELLOW)));
    EXPECT_EQ(GetState().mSelfState.mHandcards.Number(), 6);
    EXPECT_TRUE(GetState().mPlayerStates[1].mDoPlayInLastRound);
    // it should be YW instead of Y4 or W
    EXPECT_EQ(GetState().mPlayerStates[1].mLastPlayedCard, "YW");
    EXPECT_EQ(GetState().mGameState.mLastPlayedCard, "Y4");
    EXPECT_TRUE(GetState().mGameState.mIsInClockwise);
    EXPECT_EQ(GetState().mGameState.mCurrentPlayer, 2);

    UserEvent();
    EXPECT_TRUE(GetScreen()->mNotMyTurnPanel.Focused());
}

TEST_F(UnoScreenFixture, SpecifyNextColorFromPlayImmediately) {
    InitHandcardsT initHandcards = {"Y1", "Y+2", "GR", "B0", "B2", "BS", "BR"};
    GetGameStartMsg(MsgBuilder::CreateGameStart(initHandcards, "R4", 1));
    EXPECT_TRUE(GetScreen()->mPlayOrPassPanel.Focused());
    UserEvent();

    auto opRet = CoreMsgBuilder::CreateOperationInRoomRet(ErrorNumber::OK);
    EXPECT_CALL(*mMockStub, OperationInRoom(_, DrawMatcher(1), _))
        .Times(1)
        .WillOnce(DoAll(SetArgPointee<2>(opRet), Return(Status::OK)));
    UserEvent(GetScreen()->mPlayOrPassPanel.mPassButton.on_click);

    CoreMsg(CoreMsgBuilder::CreateBroadcastMsgByNotifyMsg(
        0, 0, 0, MsgBuilder::CreateDraw(1)));
    CoreMsg(CoreMsgBuilder::CreateBroadcastMsgByNotifyMsg(
        0, 0, 1, MsgBuilder::CreateDrawRsp({"+4"})));
    EXPECT_EQ(GetState().mSelfState.mHandcards.Number(), 8);
    EXPECT_EQ(GetState().mSelfState.mIndexOfNewlyDrawn, 7);
    EXPECT_TRUE(GetState().mSelfState.mHasChanceToPlayAfterDraw);
    EXPECT_TRUE(GetState().mGameState.IsMyTurn());

    UserEvent();
    EXPECT_TRUE(GetScreen()->mPlayImmediatelyPanel.Focused());
    UserEvent();

    UserEvent(GetScreen()->mPlayImmediatelyPanel.mYesButton.on_click);
    EXPECT_TRUE(GetScreen()->mSpecifyColorPanel.Focused());
    EXPECT_EQ(GetScreen()->mSpecifyColorPanel.mCursor, 7);
    UserEvent();

    // click cancel button will return to PlayImmediatelyPanel
    UserEvent(GetScreen()->mSpecifyColorPanel.mCancelButton.on_click);
    EXPECT_TRUE(GetScreen()->mPlayImmediatelyPanel.Focused());
    UserEvent();

    UserEvent(GetScreen()->mPlayImmediatelyPanel.mYesButton.on_click);
    EXPECT_TRUE(GetScreen()->mSpecifyColorPanel.Focused());
    UserEvent();

    EXPECT_CALL(*mMockStub,
                OperationInRoom(_, PlayMatcher("+4", CardColor::BLUE), _))
        .Times(1)
        .WillOnce(DoAll(SetArgPointee<2>(opRet), Return(Status::OK)));
    UserEvent(GetScreen()->mSpecifyColorPanel.mBlueButton.on_click);

    CoreMsg(CoreMsgBuilder::CreateBroadcastMsgByNotifyMsg(
        0, 0, 0, MsgBuilder::CreatePlay("+4", CardColor::BLUE)));
    EXPECT_EQ(GetState().mSelfState.mHandcards.Number(), 7);
    EXPECT_TRUE(GetState().mPlayerStates[1].mDoPlayInLastRound);
    EXPECT_EQ(GetState().mPlayerStates[1].mLastPlayedCard, "B+4");
    EXPECT_EQ(GetState().mGameState.mLastPlayedCard, "B+4");
    EXPECT_TRUE(GetState().mGameState.mIsInClockwise);
    EXPECT_EQ(GetState().mGameState.mCurrentPlayer, 2);
    EXPECT_EQ(GetState().mGameState.mCardsNumToDraw, 4);

    UserEvent();
    EXPECT_TRUE(GetScreen()->mNotMyTurnPanel.Focused());
}

TEST_F(UnoScreenFixture, DrawPenalty) {
    InitHandcardsT initHandcards = {"R1", "Y+2", "GR", "BS", "W", "Y2", "Y0"};
    GetGameStartMsg(MsgBuilder::CreateGameStart(initHandcards, "R0", 0));

    CoreMsg(CoreMsgBuilder::CreateBroadcastMsgByNotifyMsg(
        0, 0, 0, MsgBuilder::CreatePlay("+4", CardColor::YELLOW)));
    EXPECT_EQ(GetState().mPlayerStates[0].mRemainingHandCardsNum, 6);
    EXPECT_TRUE(GetState().mPlayerStates[0].mDoPlayInLastRound);
    EXPECT_EQ(GetState().mGameState.mLastPlayedCard, "Y+4");
    EXPECT_EQ(GetState().mGameState.mCardsNumToDraw, 4);
    EXPECT_TRUE(GetState().mGameState.IsMyTurn());

    UserEvent();
    EXPECT_TRUE(GetScreen()->mPlayOrPassPanel.Focused());
    UserEvent();

    auto opRet = CoreMsgBuilder::CreateOperationInRoomRet(ErrorNumber::OK);
    EXPECT_CALL(*mMockStub, OperationInRoom(_, DrawMatcher(4), _))
        .Times(1)
        .WillOnce(DoAll(SetArgPointee<2>(opRet), Return(Status::OK)));
    UserEvent(GetScreen()->mPlayOrPassPanel.mPassButton.on_click);

    CoreMsg(CoreMsgBuilder::CreateBroadcastMsgByNotifyMsg(
        0, 0, 0, MsgBuilder::CreateDraw(4)));
    CoreMsg(CoreMsgBuilder::CreateBroadcastMsgByNotifyMsg(
        0, 0, 1, MsgBuilder::CreateDrawRsp({"R5", "Y6", "GS", "+4"})));
    EXPECT_EQ(GetState().mSelfState.mHandcards.Number(), 11);
    EXPECT_FALSE(GetState().mSelfState.mHasChanceToPlayAfterDraw);
    EXPECT_TRUE(GetState().mGameState.IsMyTurn());
    EXPECT_EQ(GetState().mGameState.mCardsNumToDraw, 1);
    EXPECT_EQ(GetState().mGameState.mLastPlayedCard, "Y");

    EXPECT_CALL(*mMockStub, OperationInRoom(_, SkipMatcher(), _))
        .Times(1)
        .WillOnce(DoAll(SetArgPointee<2>(opRet), Return(Status::OK)));
    UserEvent();

    CoreMsg(CoreMsgBuilder::CreateBroadcastMsgByNotifyMsg(
        0, 0, 0, MsgBuilder::CreateSkip()));
    EXPECT_EQ(GetState().mGameState.mCurrentPlayer, 2);
    EXPECT_FALSE(GetState().mPlayerStates[1].mDoPlayInLastRound);

    UserEvent();
    EXPECT_TRUE(GetScreen()->mNotMyTurnPanel.Focused());
}
}}  // namespace SailGame::Test