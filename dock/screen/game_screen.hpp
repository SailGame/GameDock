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

    GameScreen() {}

    virtual Element Render() { return mGameScreen->Render(); }

    virtual GameType GetGameType() const { 
        return mGameScreen ? mGameScreen->GetGameType() : GameType::NoGame;
    }

    void SwitchToNewGameScreen(const std::shared_ptr<GameScreen> &gameScreen) {
        mGameScreen = gameScreen;
        mGameScreen->SetUIProxy(mUIProxy);
    }

    void ResetGameScreen() {
        mGameScreen.reset();
    }

    virtual ~GameScreen() = default;

public:
    std::shared_ptr<GameScreen> mGameScreen;

};
}}