#pragma once

#include <sailgame/common/state_machine.h>
#include <sailgame/common/event.h>
#include <sailgame_pb/core/types.pb.h>
#include <sailgame_pb/uno/uno.pb.h>

#include "state.h"

namespace SailGame { namespace Uno {

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

protected:
    virtual void Transition(const BroadcastMsg &msg) override;

private:
    void Transition(const NotifyMsg &msg);

    void Transition(const Draw &msg);

    void Transition(const DrawRsp &msg);

    void Transition(const Skip &msg);

    void Transition(const Play &msg);

private:
    WholeState mState;
};
}}