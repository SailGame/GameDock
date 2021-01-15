#pragma once

#include <sailgame_pb/core/types.pb.h>

namespace SailGame { namespace Dock {

using ::Core::RoomDetails;

class DockUtil {
public:
    static std::vector<std::string> GetUsernamesFromRoomDetails(
        const RoomDetails &details)
    {
        std::vector<std::string> usernames;
        for (const auto &roomUser : details.user()) {
            usernames.push_back(roomUser.username());
        }
        return usernames;
    }

};

}}