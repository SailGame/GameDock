#pragma once

#include <sailgame/common/state_machine.h>
#include <sailgame/common/types.h>
#include <sailgame/common/util.h>
#include <sailgame_pb/core/types.pb.h>

namespace SailGame { namespace Dock {

using Common::GameType;
using Common::IState;
using Common::Util;
using ::Core::RoomDetails;
using ::Core::RoomUser;

class State : public IState {
public:
    State() = default;

    bool IsOwner() const {
        /// FIXME: shouldn't assume that owner's username appears as the first
        /// one
        // return (mRoomDetails.user_size() > 0) &&
        //        (mRoomDetails.user(0).username() == mUsername);

        /// XXX: currently there is no owner
        return false;
    }

    bool AreAllUsersReady() const {
        auto roomUsers = mRoomDetails.user();
        if (roomUsers.empty()) {
            return false;
        }
        for (const auto &roomUser : roomUsers) {
            // in Core side, all players status will change to PLAYING
            // automatically if they are all ready
            if (roomUser.userstate() != RoomUser::PLAYING) {
                return false;
            }
        }
        return true;
    }

    GameType GetGameType() const {
        return Util::GetGameTypeByGameName(mRoomDetails.gamename());
    }

public:
    std::string mUsername;
    RoomDetails mRoomDetails;
};
}}  // namespace SailGame::Dock