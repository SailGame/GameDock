#pragma once

#include <sailgame_pb/core/types.pb.h>
#include <sailgame/common/state_machine.h>

namespace SailGame { namespace Dock {

using Common::IState;
using ::Core::RoomDetails;

class State : public IState {
public:
    State(const std::string &username) : mUsername(username) {}

    bool IsOwner() const {
        // assume that owner's username appears as the first one
        return (mRoomDetails.user_size() > 0) &&
               (mRoomDetails.user(0).username() == mUsername);
    }

public:
    std::string mUsername;
    RoomDetails mRoomDetails;
};
}}