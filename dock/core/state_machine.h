#pragma once

#include <sailgame/common/state_machine.h>

#include "state.h"

namespace SailGame { namespace Dock {

using ::Core::BroadcastMsg;
using ::Core::RoomDetails;
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
    virtual void Transition(const BroadcastMsg &msg) override {
        switch (msg.Msg_case()) {
            case BroadcastMsg::MsgCase::kRoomDetails:
                Transition(msg.roomdetails());
                return;
            /// TODO: handle other cases
        }
        throw std::runtime_error("Unsupported msg type");
    }

private:
    void Transition(const RoomDetails &details) {
        mState.mRoomDetails = details;
    }

private:
    State mState;
};
}}