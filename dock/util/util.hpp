#pragma once

#include <sailgame_pb/core/types.pb.h>

namespace SailGame { namespace Dock {

using ::Core::RoomDetails;

class DockUtil {
public:
    static std::vector<std::string> GetUsernamesFromRoomDetails(
        const RoomDetails &details) {
        std::vector<std::string> usernames;
        for (const auto &roomUser : details.user()) {
            usernames.push_back(roomUser.username());
        }
        return usernames;
    }

    // If str is all number, return it as roomId.
    // otherwise return -1
    static int TryToGetRoomId(const std::string &str) {
        for (auto c : str) {
            if (c < '0' || c > '9') {
                return -1;
            }
        }
        return std::stoi(str);
    }
};

}}  // namespace SailGame::Dock