#pragma once

#include <iostream>
#include <vector>
#include <spdlog/spdlog.h>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>

#include "core/ui_proxy.h"

#include "screen/lobby_screen.hpp"
#include "screen/login_screen.hpp"
#include "screen/room_screen.hpp"

namespace SailGame { namespace Dock {

using SailGame::Dock::IUIProxy;
using SailGame::Dock::MockUIProxy;

class Dock : public ftxui::Component {
public:
    Dock(const std::shared_ptr<IUIProxy> &uiProxy);

    ~Dock();

    // FtxUI
    void Loop();

// private:
public:
    ftxui::ScreenInteractive mScreen{ScreenInteractive::Fullscreen()};
    
    Container mScreenContainer{Container::Tab(nullptr)};
    std::shared_ptr<IUIProxy> mUIProxy;
    LoginScreen mLoginScreen;
    LobbyScreen mLobbyScreen;
    RoomScreen mRoomScreen;
};

}}