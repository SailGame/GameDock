#pragma once

#include <ftxui/component/menu.hpp>
#include <ftxui/screen/string.hpp>
#include <sailgame_pb/core/types.pb.h>
#include <sailgame/common/util.h>
#include <sailgame/common/state_machine.h>

#include "../../games/uno/src/util.h"
#include "../../games/uno/src/state_machine.h"
#include "../../games/uno/src/game_screen.h"
#include "../screen/game_screen.hpp"
#include "../core/state_machine.h"

namespace SailGame { namespace Dock {

using Common::IStateMachine;
using ::Core::Room;
using ::Core::RoomDetails;
using ::Core::RoomUser;
using ftxui::Element;
using ftxui::Menu;
using ftxui::text;
using ftxui::to_wstring;
using ftxui::vbox;
using Uno::UnoUtil;

class DockUtil {
public:
    /// TODO: put these to-* functions to another class
    static std::wstring RoomToString(const Room &room) {
        return to_wstring(room.roomid()) + L"    " +
            to_wstring(room.gamename());
    }

    static Element UserNameToText(const std::string &username) {
        return text(to_wstring(username));
    }

    static Element RoomUserToText(const RoomUser &roomUser) {
        return text(to_wstring(roomUser.username()) + L"    " +
            RoomUserStateToString(roomUser.userstate()));
    }

    static std::wstring RoomUserStateToString(RoomUser::RoomUserState state) {
        switch (state) {
            case RoomUser::ERROR:        return L"Error";
            case RoomUser::PREPARING:    return L"Preparing";
            case RoomUser::READY:        return L"Ready";
            case RoomUser::DISCONNECTED: return L"Disconnected";
            case RoomUser::PLAYING:      return L"Playing";
        }
        throw std::runtime_error("Unsupported RoomUserState.");
    }

    template<typename ElemT, typename FuncT>
    static void MapVectorToMenuEntries(ftxui::Menu &menu,
        const std::vector<ElemT> &vec, const FuncT &func) 
    {
        menu.entries.clear();
        for (const auto &element : vec) {
            menu.entries.push_back(func(element));
        }
    }

    template<typename VecT, typename FuncT>
    // VecT could be std::vector or grpc RepeatedField
    static Element MapVectorToVBox(const VecT &vec,
        const FuncT &func)
    {
        auto vBox = vbox({});
        for (const auto &element : vec) {
            vBox->children.push_back(func(element));
        }
        return vBox;
    }

    static std::vector<std::string> GetUsernamesFromRoomDetails(
        const RoomDetails &details)
    {
        std::vector<std::string> usernames;
        for (const auto &roomUser : details.user()) {
            usernames.push_back(roomUser.username());
        }
        return usernames;
    }

    static Element ShowGameSettings(const RoomDetails &details) {
        if (details.gamename() == "") {
            return text(L"empty");
        }
        if (details.gamename() == "UNO") {
            return UnoUtil::ShowGameSettings(details.gamesetting());
        }
        /// TODO: handle other games
        throw std::runtime_error("Unsupported game.");
    }

    static std::shared_ptr<IStateMachine> GetStateMachineByGameName(
        const std::string gameName)
    {
        if (gameName == "UNO") {
            return Uno::StateMachine::Create();
        }
        /// TODO: support other games
        throw std::runtime_error("Unsupported game.");
    }

    static std::shared_ptr<GameScreen> GetGameScreenByGameName(
        const std::string gameName)
    {
        if (gameName == "UNO") {
            return Uno::GameScreen::Create();
        }
        /// TODO: support other games
        throw std::runtime_error("Unsupported game.");
    }
};

}}