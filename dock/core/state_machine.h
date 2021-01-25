#pragma once

#include <spdlog/spdlog.h>
#include <sailgame/common/state_machine.h>

#include "state.h"
#include "common.h"

namespace SailGame { namespace Dock {

using ::Core::BroadcastMsg;
using ::Core::RoomDetails;
using Common::IStateMachine;
using Common::ClientStateMachine;
using Common::IState;
using Common::GameType;

class StateMachine : public ClientStateMachine {
public:
    // let StateMachine check whether game starts
    // because in Dock level, we could only check that in Render, 
    // which is however asyncly invoked.
    std::function<void(GameType)> OnGameStart;

    StateMachine() = default;

    static std::shared_ptr<ClientStateMachine> Create() {
        return std::make_shared<StateMachine>();
    }

    const IState &GetState() const override { return mState; }

    virtual void SetState(const IState &state) {
        mState = dynamic_cast<const State &>(state);
    }

    GameType GetType() const override { return GameType::NoGame; }

    void SwitchFrom(const ClientStateMachine &stateMachine) override {
        mState = stateMachine.SwitchToRoom();
        for (auto i = 0; i< mState.mRoomDetails.user_size(); i++) {
            mState.mRoomDetails.mutable_user(i)->set_userstate(
                RoomUser::PREPARING);
        }
    }

    State SwitchToRoom() const override {
        throw std::runtime_error(
            "Dock::StateMachine shouldn't switch from itself");
    }

protected:
    virtual void Transition(const BroadcastMsg &msg) override {
        spdlog::info("[Dock StateMachine] Transition, msg case: {}",
                     msg.Msg_case());
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
        /// XXX: for now, ret from Core doesn't include gameSetting
        mState.mRoomDetails.mutable_gamesetting()->PackFrom(
            Uno::MsgBuilder::CreateStartGameSettings(
                true, true, false, false, 15));
        if (mState.AreAllUsersReady()) {
            spdlog::info("[Dock StateMachine] All users are ready");
            OnGameStart(mState.GetGameType());
        }
    }

private:
    State mState;
};

}}