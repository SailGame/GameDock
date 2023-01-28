#pragma once

#include <spdlog/spdlog.h>

#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <iostream>
#include <memory>
#include <vector>

#include "component/poly_component.hpp"
#include "screen/game_screen.hpp"
#include "screen/lobby_screen.hpp"
#include "screen/login_screen.hpp"
#include "screen/room_screen.hpp"

namespace SailGame { namespace Dock {

using SailGame::Dock::UIProxy;

class Dock {
public:
    Dock(const std::shared_ptr<UIProxy> &uiProxy);

    ~Dock() = default;

    void Loop(bool useRefersher = true);

private:
    void QueryRoomAndSetStateMachine(int roomId);

public:
    ftxui::ScreenInteractive mScreen{ScreenInteractive::Fullscreen()};

    int mScreenContainerSelected{0};
    Component mScreenContainer;
    std::shared_ptr<UIProxy> mUIProxy;
    std::shared_ptr<LoginScreen> mLoginScreen;
    std::shared_ptr<LobbyScreen> mLobbyScreen;
    std::shared_ptr<RoomScreen> mRoomScreen;
    std::shared_ptr<PolyComponent<GameScreen>> mPolyGameScreen;
};

}}  // namespace SailGame::Dock