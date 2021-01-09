#pragma once

#include <sailgame_pb/core/types.pb.h>
#include <sailgame/common/state_machine.h>

namespace SailGame { namespace Dock {

using Common::IState;
using ::Core::RoomDetails;

class State : public IState {
public:
    State() = default;

public:
    RoomDetails mRoomDetails;
};
}}