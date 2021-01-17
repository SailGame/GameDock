#pragma once

#include <memory>
#include <ftxui/dom/elements.hpp>
#include <google/protobuf/any.pb.h>
#include <sailgame/common/state_machine.h>

#include "../screen/game_screen.hpp"
#include "../component/game_settings_ctrl.hpp"

namespace SailGame { namespace Dock {

using Common::IStateMachine;
using ftxui::Element;
using google::protobuf::Any;

class IGameAttr {
public:
    virtual std::shared_ptr<IStateMachine> GetStateMachine() = 0;

    virtual std::shared_ptr<GameScreen> GetGameScreen() = 0;

    virtual std::shared_ptr<GameSettingsController>
        GetGameSettingsController() = 0;

    virtual Element ShowGameSettings(const Any &any) = 0;
};
}}