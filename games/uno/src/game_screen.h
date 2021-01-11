#pragma once

#include <ftxui/component/container.hpp>
#include <sailgame/common/types.h>
#include "../../../dock/screen/game_screen.hpp"
#include "state.h"

namespace SailGame { namespace Uno {

using namespace ftxui;
using Common::GameType;

class GameScreen : public Dock::GameScreen {
public:
    GameScreen() {
        Add(&mContainer);
        // mContainer.Add()
    }

    static std::shared_ptr<Dock::GameScreen> Create() {
        return std::make_shared<GameScreen>();
    }

    virtual GameType GetGameType() const override { return GameType::Uno; }

    void Update() {}

    Element Render() {
        Update();

        auto doc = hbox({text(L"hello"), text(L"uno")});
        return doc | border | center;
    }

    WholeState GetState() const {
        return dynamic_cast<const WholeState &>(mUIProxy->GetState());
    }

public:
    Container mContainer{Container::Horizontal()};
};

}}