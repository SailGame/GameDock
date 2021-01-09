#pragma once

#include <sailgame/common/state_machine.h>

#include "state.h"

namespace SailGame { namespace Dock {

using Common::IStateMachine;
using Common::IState;

class StateMachine : public IStateMachine {
public:
    StateMachine() = default;

    static std::shared_ptr<IStateMachine> Create() {
        return std::make_shared<StateMachine>();
    }

    const IState &GetState() const override { return mState; }

protected:
   private:
    State mState;
};
}}