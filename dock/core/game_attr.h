#pragma once

#include <google/protobuf/any.pb.h>
#include <sailgame/common/state_machine.h>

#include <ftxui/dom/elements.hpp>
#include <memory>

#include "../component/game_settings_ctrl.hpp"
#include "../screen/game_screen.hpp"

namespace SailGame { namespace Dock {

using Common::ClientStateMachine;
using ftxui::Element;
using google::protobuf::Any;

class IGameAttr {
public:
    virtual std::shared_ptr<ClientStateMachine> GetStateMachine() = 0;

    virtual std::shared_ptr<GameScreen> GetGameScreen() = 0;

    virtual std::shared_ptr<GameSettingsController>
    GetGameSettingsController() = 0;

    virtual Element ShowGameSettings(const Any &any) = 0;
};
}}  // namespace SailGame::Dock