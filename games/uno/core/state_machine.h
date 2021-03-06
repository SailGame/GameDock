#pragma once

#include <sailgame/common/event.h>
#include <sailgame/common/state_machine.h>
#include <sailgame/common/types.h>
#include <sailgame_pb/core/types.pb.h>
#include <sailgame_pb/uno/uno.pb.h>

#include "../../../dock/core/common.h"
#include "state.h"

namespace SailGame { namespace Uno {

using Common::ClientStateMachine;
using Common::GameType;
using Common::IStateMachine;
using namespace ::Core;
using namespace ::Uno;

class StateMachine : public ClientStateMachine {
public:
    StateMachine() = default;

    static std::shared_ptr<ClientStateMachine> Create() {
        return std::make_shared<StateMachine>();
    }

    const IState &GetState() const override { return mState; }

    void SetState(const IState &state) override {
        mState = dynamic_cast<const WholeState &>(state);
    }

    GameType GetType() const override { return GameType::Uno; }

    void SwitchFrom(const ClientStateMachine &stateMachine) override;

    Dock::State SwitchToRoom() const override;

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
}}  // namespace SailGame::Uno