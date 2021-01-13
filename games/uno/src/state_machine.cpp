#include <sailgame_pb/core/provider.pb.h>
#include <sailgame_pb/core/error.pb.h>
#include <sailgame_pb/uno/uno.pb.h>

#include <sailgame/common/state_machine.h>
#include <sailgame/common/util.h>
#include <sailgame/common/event.h>
#include <sailgame/common/core_msg_builder.h>
#include <sailgame/uno/msg_builder.h>

#include "state_machine.h"

namespace SailGame { namespace Uno {

using Common::CoreMsgBuilder;
using Common::Util;

void StateMachine::Transition(const BroadcastMsg &msg)
{
    switch (msg.Msg_case()) {
        case BroadcastMsg::MsgCase::kCustom:
            Transition(Common::Util::UnpackGrpcAnyTo<NotifyMsg>(msg.custom()));
            return;
        /// TODO: handle other cases
    }
    throw std::runtime_error("Unsupported msg type");
}

void StateMachine::Transition(const NotifyMsg &msg)
{
    switch (msg.Msg_case()) {
        case NotifyMsg::MsgCase::kGameStart:
            Transition(msg.gamestart()); return;
        case NotifyMsg::MsgCase::kDraw:
            Transition(msg.draw()); return;
        case NotifyMsg::MsgCase::kDrawRsp:
            Transition(msg.drawrsp()); return;
        case NotifyMsg::MsgCase::kSkip:
            Transition(msg.skip()); return;
        case NotifyMsg::MsgCase::kPlay:
            Transition(msg.play()); return;
        /// TODO: handle other cases
    }
    throw std::runtime_error("Unsupported msg type");
}

void StateMachine::Transition(const GameStart &msg)
{
    auto &gameState = mState.mGameState;
    gameState.mCurrentPlayer = msg.firstplayer();
    gameState.mIsInClockwise =
        Card{msg.flippedcard()}.mText != CardText::REVERSE;
    gameState.mLastPlayedCard = Card{msg.flippedcard()};

    auto &selfState = mState.mSelfState;
    for (auto card : msg.inithandcards()) {
        selfState.mHandcards.Draw(Card{card});
    }

    auto initHandcardsNum = 7;
    for (auto &playerState : mState.mPlayerStates) {
        playerState.mRemainingHandCardsNum = initHandcardsNum;
    }
}

void StateMachine::Transition(const Draw &msg)
{
    assert(mState.mGameState.mCardsNumToDraw == msg.number());
    mState.mPlayerStates[mState.mGameState.mCurrentPlayer].UpdateAfterDraw(
        msg.number());

    // GameState should update in the last because it may change current player.
    mState.mGameState.UpdateAfterDraw();
}

// OperationInRoomArgs StateMachine::Transition(const DrawEvent &event)
// {
//     assert(mState.mGameState.mCurrentPlayer == mState.mGameState.mSelfPlayerIndex);
//     auto number = mState.mGameState.mCardsNumToDraw;
//     mState.mPlayerStates[mState.mGameState.mCurrentPlayer].UpdateAfterDraw(
//         number);

//     mState.mGameState.UpdateAfterDraw();
//     return CoreMsgBuilder::CreateOperationInRoomArgs(
//         MsgBuilder::CreateDraw(number));
// }

void StateMachine::Transition(const DrawRsp &msg)
{
    assert(mState.mGameState.IsMyTurn());
    auto grpcCards = Common::Util::ConvertGrpcRepeatedPtrFieldToVector(msg.cards());

    std::vector<Card> cards;
    for (auto card : grpcCards) {
        cards.emplace_back(card);
    }
    mState.mSelfState.UpdateAfterDrawRsp(cards);
}

void StateMachine::Transition(const Skip &msg)
{
    mState.mPlayerStates[mState.mGameState.mCurrentPlayer].UpdateAfterSkip();
    mState.mGameState.UpdateAfterSkip();
}

// OperationInRoomArgs StateMachine::Transition(const SkipEvent &event)
// {
//     assert(mState.mGameState.mCurrentPlayer == mState.mGameState.mSelfPlayerIndex);
//     mState.mSelfState.UpdateAfterSkip();
//     mState.mPlayerStates[mState.mGameState.mCurrentPlayer].UpdateAfterSkip();
//     mState.mGameState.UpdateAfterSkip();
//     return CoreMsgBuilder::CreateOperationInRoomArgs(MsgBuilder::CreateSkip());
// }

void StateMachine::Transition(const Play &msg)
{
    auto card = msg.card();
    if (mState.mGameState.IsMyTurn()) {
        mState.mSelfState.UpdateAfterPlay(card);
    }
    mState.mPlayerStates[mState.mGameState.mCurrentPlayer].UpdateAfterPlay(card);
    mState.mGameState.UpdateAfterPlay(card);
}

// OperationInRoomArgs StateMachine::Transition(const PlayEvent &event)
// {
//     assert(mState.mGameState.mCurrentPlayer == mState.mGameState.mSelfPlayerIndex);
//     auto card = event.mCard;
//     mState.mSelfState.UpdateAfterPlay(card);
//     card.mColor = event.mNextColor;
//     mState.mPlayerStates[mState.mGameState.mCurrentPlayer].UpdateAfterPlay(card);
//     mState.mGameState.UpdateAfterPlay(card);
//     return CoreMsgBuilder::CreateOperationInRoomArgs(
//         MsgBuilder::CreatePlay(event.mCard, event.mNextColor));
// }

}}