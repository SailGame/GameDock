#include <sailgame_pb/core/provider.pb.h>
#include <sailgame_pb/core/error.pb.h>
#include <sailgame_pb/uno/uno.pb.h>

#include <sailgame/common/state_machine.h>
#include <sailgame/common/util.h>
#include <sailgame/uno/msg_builder.h>

#include "state.h"

namespace SailGame { namespace Common {

using Game::Card;
using Game::CardSet;
using Game::GameState;
using Game::WholeState;
using Game::MsgBuilder;
using Core::ProviderMsg;
using Core::BroadcastMsg;
using Core::RegisterRet;
using Core::StartGameArgs;
using Core::CloseGameArgs;
using Core::QueryStateArgs;
using Core::UserOperationArgs;
using Core::NotifyMsgArgs;
using ::Uno::StartGameSettings;
using ::Uno::UserOperation;
using ::Uno::NotifyMsg;
using ::Uno::Draw;
using ::Uno::DrawRsp;
using ::Uno::Skip;
using ::Uno::Play;
using ::Uno::Uno;
using ::Uno::Catch;
using ::Uno::Doubt;
using ::Uno::GameStart;
using ::Uno::Exit;
using ::Uno::CardColor;
using ::Uno::CardText;

#define TransitionForMsg(MsgT) \
    template<> \
    template<> \
    void StateMachine<WholeState>::TransitionForBroadcastMsg<MsgT>(const MsgT &msg)

#define TransitionForEvent(EventT) \
    template<> \
    template<> \
    OperationInRoomArgsPtr StateMachine<WholeState>::TransitionForUserInput<EventT>( \
        const EventT &event)

TransitionForMsg(BroadcastMsg);

TransitionForMsg(NotifyMsg);
TransitionForMsg(Draw);
TransitionForMsg(DrawRsp);
TransitionForMsg(Skip);
TransitionForMsg(Play);

TransitionForMsg(BroadcastMsg)
{
    switch (msg.Msg_case()) {
        case BroadcastMsg::MsgCase::kCustom:
            TransitionForBroadcastMsg<NotifyMsg>(
                Util::UnpackGrpcAnyTo<NotifyMsg>(msg.custom()));
            break;
        /// TODO: handle other cases
    }
    throw std::runtime_error("Unsupported msg type");
}

TransitionForMsg(NotifyMsg)
{
    switch (msg.Msg_case()) {
        case NotifyMsg::MsgCase::kDraw:
            TransitionForBroadcastMsg<Draw>(msg.draw()); break;
        case NotifyMsg::MsgCase::kDrawRsp:
            TransitionForBroadcastMsg<DrawRsp>(msg.drawrsp()); break;
        case NotifyMsg::MsgCase::kSkip:
            TransitionForBroadcastMsg<Skip>(msg.skip()); break;
        case NotifyMsg::MsgCase::kPlay:
            TransitionForBroadcastMsg<Play>(msg.play()); break;
        /// TODO: handle other cases
    }
    throw std::runtime_error("Unsupported msg type");
}

TransitionForMsg(Draw)
{
    mState.mPlayerStates[mState.mGameState.mCurrentPlayer].UpdateAfterDraw(
        msg.number());

    // GameState should update in the last because it may change current player.
    mState.mGameState.UpdateAfterDraw();
}

TransitionForMsg(DrawRsp)
{
    assert(mState.mGameState.IsMyTurn());
    auto grpcCards = Util::ConvertGrpcRepeatedPtrFieldToVector(msg.cards());

    std::vector<Card> cards;
    for (auto card : grpcCards) {
        cards.emplace_back(card);
    }
    mState.mSelfState.UpdateAfterDrawRsp(cards);
}

TransitionForMsg(Skip)
{
    if (mState.mGameState.IsMyTurn()) {
        mState.mSelfState.UpdateAfterSkip();
    }
    mState.mPlayerStates[mState.mGameState.mCurrentPlayer].UpdateAfterSkip();
    mState.mGameState.UpdateAfterSkip();
}

TransitionForMsg(Play)
{
    auto card = msg.card();
    if (mState.mGameState.IsMyTurn()) {
        mState.mSelfState.UpdateAfterPlay(card);
    }
    mState.mPlayerStates[mState.mGameState.mCurrentPlayer].UpdateAfterPlay(card);
    mState.mGameState.UpdateAfterPlay(card);
}
}}