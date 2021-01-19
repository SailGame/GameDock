#pragma once

#include <sailgame/common/state_machine.h>

#include "state.h"

namespace SailGame { namespace Dock {

using ::Core::BroadcastMsg;
using ::Core::RoomDetails;
using Common::IStateMachine;
using Common::ClientStateMachine;
using Common::IState;
using Common::GameType;

class StateMachine : public ClientStateMachine {
public:
    StateMachine(const std::string &username) : mState(username) {}

    static std::shared_ptr<ClientStateMachine> Create(const std::string &username) {
        return std::make_shared<StateMachine>(username);
    }

    const IState &GetState() const override { return mState; }

    virtual void SetState(const IState &state) {
        mState = dynamic_cast<const State &>(state);
    }

    GameType GetType() const override { return GameType::NoGame; }

    void SwitchFrom(const IStateMachine &) override {
        /// TODO: state machine switches from game to room
    }

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