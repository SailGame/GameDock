#pragma once

#include <sailgame/common/event.h>
#include <sailgame/common/state_machine.h>
#include <sailgame/common/types.h>
#include <sailgame_pb/core/types.pb.h>
#include <sailgame_pb/texas/texas.pb.h>

#include "../../../dock/core/common.h"

namespace SailGame { namespace Texas {

using Common::ClientStateMachine;
using Common::GameType;
using Common::IState;
using Common::IStateMachine;
using namespace ::Core;

struct State : IState {};

class StateMachine : public ClientStateMachine {
public:
    StateMachine() = default;

    static std::shared_ptr<ClientStateMachine> Create() {
        return std::make_shared<StateMachine>();
    }

    const IState &GetState() const override { return mState; }

    void SetState(const IState &state) override {}

    GameType GetType() const override { return GameType::Texas; }

    void SwitchFrom(const ClientStateMachine &stateMachine) override;

    Dock::State SwitchToRoom() const override;

protected:
    virtual void Transition(const BroadcastMsg &msg) override;

private:
    State mState;
};
}}  // namespace SailGame::Texas