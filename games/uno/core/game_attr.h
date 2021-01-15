#pragma once

#include <google/protobuf/any.pb.h>
#include "../../../dock/core/game_attr.h"
#include "../ui/game_screen.hpp"
#include "../ui/dom.hpp"
#include "state_machine.h"

namespace SailGame { namespace Uno {

using Common::IStateMachine;
using Dock::IGameAttr;
using ftxui::Element;
using google::protobuf::Any;

class GameAttr : public IGameAttr {
public:
    virtual std::shared_ptr<IStateMachine> GetStateMachine() override {
        return StateMachine::Create();
    }

    virtual std::shared_ptr<Dock::GameScreen> GetGameScreen() override {
        return GameScreen::Create();
    }

    virtual Element ShowGameSettings(const Any &any) override {
        return Dom::ShowGameSettings(any);
    }
};

}}