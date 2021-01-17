#pragma once

#include <ftxui/component/component.hpp>
#include <sailgame/common/types.h>

#include "../core/ui_proxy.h"

namespace SailGame { namespace Dock {

using namespace ftxui;
using Common::GameType;

class GameScreen : public Component, public UIProxyClient {
public:
    std::function<void()> OnGameOver;

    GameScreen() = default;

    virtual GameType GetGameType() const { 
        return GameType::NoGame;
    }

    void RegisterGameOverCallback(const std::function<void()> &callback) {
        OnGameOver = callback;
    }
};
}}