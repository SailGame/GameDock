#pragma once

#include <sailgame/common/types.h>

#include <ftxui/component/component.hpp>

#include "../core/component.h"

namespace SailGame { namespace Dock {

using namespace ftxui;
using Common::GameType;

class GameScreen : public ComponentWithUIProxy {
public:
    std::function<void()> OnGameOver;

    void RegisterGameOverCallback(const std::function<void()> &callback) {
        OnGameOver = callback;
    }

    GameScreen() = default;

    virtual GameType GetGameType() const = 0;
};
}}  // namespace SailGame::Dock