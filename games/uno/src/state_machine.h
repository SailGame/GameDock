#pragma once

#include <sailgame/common/state_machine.h>
#include <sailgame/common/event.h>
#include <sailgame/common/types.h>
#include <sailgame_pb/core/types.pb.h>
#include <sailgame_pb/uno/uno.pb.h>

#include "state.h"
#include "util.h"
#include "../../../dock/core/state_machine.h"

namespace SailGame { namespace Uno {

using Common::GameType;
using Common::IStateMachine;
using namespace ::Core;
using namespace ::Uno;

class StateMachine : public IStateMachine {
public:
    StateMachine() = default;

    static std::shared_ptr<IStateMachine> Create() {
        return std::make_shared<StateMachine>();
    }

    const IState &GetState() const override { return mState; }

    GameType GetType() const override { return GameType::Uno; }

    void InheritFrom(const IStateMachine &stateMachine) override {
        auto roomState =
            dynamic_cast<const Dock::State &>(stateMachine.GetState());
        auto roomDetails = roomState.mRoomDetails;
        for (auto roomUser : roomDetails.user()) {
            mState.mPlayerStates.emplace_back(roomUser.username());
        }
        mState.mGameState.mPlayerNum = roomDetails.user_size();
        mState.mGameState.mGameSettings =
            UnoUtil::GetGameSettings(roomDetails.gamesetting());
        for (auto i = 0; i < roomDetails.user_size(); i++) {
            if (roomState.mUsername == roomDetails.user(i).username()) {
                mState.mGameState.mSelfPlayerIndex = i;
            }
        }
    }

protected:
    virtual void Transition(const BroadcastMsg &msg) override;

private:
    void Transition(const NotifyMsg &msg);

    void Transition(const GameStart &msg);

    void Transition(const Draw &msg);

    void Transition(const DrawRsp &msg);

    void Transition(const Skip &msg);

    void Transition(const Play &msg);

private:
    WholeState mState;
};
}}