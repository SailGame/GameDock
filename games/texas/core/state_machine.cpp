#include "state_machine.h"

#include <sailgame/common/core_msg_builder.h>
#include <sailgame/common/event.h>
#include <sailgame/common/state_machine.h>
#include <sailgame/common/util.h>
// #include <sailgame/uno/msg_builder.h>
#include <sailgame_pb/core/error.pb.h>
#include <sailgame_pb/core/provider.pb.h>
#include <sailgame_pb/texas/texas.pb.h>
#include <spdlog/spdlog.h>

namespace SailGame { namespace Texas {

using Common::CoreMsgBuilder;
using Common::Util;

void StateMachine::SwitchFrom(const ClientStateMachine &stateMachine) {}

// Dock::State StateMachine::SwitchToRoom() const { return mState.mRoomState; }

void StateMachine::Transition(const BroadcastMsg &msg) {
    switch (msg.Msg_case()) {
        case BroadcastMsg::MsgCase::kCustom:
            // Transition(Common::Util::UnpackGrpcAnyTo<Texas::NotifyMsg>(msg.custom()));
            return;
        case BroadcastMsg::MsgCase::kRoomDetails:
            // this branch can be reached: you're staying in the GameOverPanel
            // when someone has returned to RoomScreen and got ready.
            return;
            /// TODO: handle other cases
    }
    throw std::runtime_error("Unsupported msg type");
}

// void StateMachine::Transition(const NotifyMsg &msg) {
//     // switch (msg.Msg_case()) {
//     //     case NotifyMsg::MsgCase::kGameStart:
//     //         Transition(msg.gamestart());
//     //         return;
//     //         /// TODO: handle other cases
//     // }
//     throw std::runtime_error("Unsupported msg type");
// }

}}  // namespace SailGame::Texas