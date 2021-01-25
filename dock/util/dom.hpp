#pragma once

#include <sailgame_pb/core/types.pb.h>

#include <ftxui/component/menu.hpp>
#include <ftxui/dom/elements.hpp>

#include "../core/game_attr_fac.h"

namespace ftxui {
Decorator range(int width, int height);
Decorator width(int value);
Decorator height(int value);
}  // namespace ftxui

namespace SailGame { namespace Dock {

using namespace ftxui;
using ::Core::Room;
using ::Core::RoomDetails;
using ::Core::RoomUser;

class Dom {
public:
    static Element ShowGameSettings(const RoomDetails &details) {
        if (details.gamename().empty()) {
            return text(L"empty");
        }
        return GameAttrFactory::Create(details.gamename())
            ->ShowGameSettings(details.gamesetting());
    }

    template <typename ElemT, typename FuncT>
    static void MapVectorToMenuEntries(ftxui::Menu &menu,
                                       const std::vector<ElemT> &vec,
                                       const FuncT &func) {
        menu.entries.clear();
        for (const auto &element : vec) {
            menu.entries.push_back(func(element));
        }
    }

    template <typename VecT, typename FuncT>
    // VecT could be std::vector or grpc RepeatedField
    static Element MapVectorToVBox(const VecT &vec, const FuncT &func) {
        auto vBox = vbox({});
        for (const auto &element : vec) {
            vBox->children.push_back(func(element));
        }
        return vBox;
    }

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

    static Element RoomUserToHBox(const RoomUser &roomUser) {
        return hbox({text(to_wstring(roomUser.username())) | flex,
                     text(RoomUserStateToString(roomUser.userstate()))});
    }

    static std::wstring RoomUserStateToString(RoomUser::RoomUserState state) {
        switch (state) {
            case RoomUser::ERROR:
                return L"Error";
            case RoomUser::PREPARING:
                return L"Preparing";
            case RoomUser::READY:
                return L"Ready";
            case RoomUser::DISCONNECTED:
                return L"Disconnected";
            case RoomUser::PLAYING:
                return L"Playing";
        }
        throw std::runtime_error("Unsupported RoomUserState.");
    }
};

}}  // namespace SailGame::Dock