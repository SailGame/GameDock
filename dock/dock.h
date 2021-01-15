#pragma once

#include <iostream>
#include <vector>
#include <spdlog/spdlog.h>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

#include "screen/lobby_screen.hpp"
#include "screen/login_screen.hpp"
#include "screen/room_screen.hpp"
#include "screen/game_screen.hpp"

namespace SailGame { namespace Dock {

using SailGame::Dock::UIProxy;

class Dock : public ftxui::Component {
public:
    Dock(const std::shared_ptr<UIProxy> &uiProxy);

    ~Dock();

    // FtxUI
    void Loop();

// private:
public:
    ftxui::ScreenInteractive mScreen{ScreenInteractive::Fullscreen()};
    
    Container mScreenContainer{Container::Tab(nullptr)};
    std::shared_ptr<UIProxy> mUIProxy;
    LoginScreen mLoginScreen;
    LobbyScreen mLobbyScreen;
    RoomScreen mRoomScreen;
    GameScreen mGameScreen;
};

}}