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
using ::Uno::StartGameSettings;

void StateMachine::SwitchFrom(const IStateMachine &stateMachine)
{
    auto roomState =
        dynamic_cast<const Dock::State &>(stateMachine.GetState());
    auto roomDetails = roomState.mRoomDetails;
    for (auto roomUser : roomDetails.user()) {
        mState.mPlayerStates.emplace_back(roomUser.username());
    }
    mState.mGameState.mPlayerNum = roomDetails.user_size();
    mState.mGameState.mGameSettings =
        Common::Util::UnpackGrpcAnyTo<StartGameSettings>(
            roomDetails.gamesetting());
    for (auto i = 0; i < roomDetails.user_size(); i++) {
        if (roomState.mUsername == roomDetails.user(i).username()) {
            mState.mGameState.mSelfPlayerIndex = i;
        }
    }
}

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
    if (mState.mGameState.IsMyTurn()) {
        mState.mSelfState.UpdateAfterSkip();
    }
    mState.mPlayerStates[mState.mGameState.mCurrentPlayer].UpdateAfterSkip();
    mState.mGameState.UpdateAfterSkip();
}

void StateMachine::Transition(const Play &msg)
{
    Card card = msg.card();
    if (mState.mGameState.IsMyTurn()) {
        mState.mSelfState.UpdateAfterPlay(card);
    }
    // if card is not wild, nextColor is certainly the same with card's color
    // otherwise, nextColor indicates the next color.
    // NOTE: color should be attached after updating SelfState because 
    // we need to erase the card just played from handcards when updating SelfState
    card.mColor = msg.nextcolor();
    mState.mPlayerStates[mState.mGameState.mCurrentPlayer].UpdateAfterPlay(card);
    mState.mGameState.UpdateAfterPlay(card);
}

}}