#pragma once

#include <google/protobuf/any.pb.h>
#include <sailgame/common/util.h>
#include <sailgame_pb/texas/texas.pb.h>

#include "../../../dock/component/game_settings_ctrl.hpp"
#include "../../../dock/core/game_attr.h"
#include "../ui/component/game_settings_ctrl.hpp"
#include "../ui/dom.hpp"
#include "../ui/game_screen.hpp"
#include "state_machine.h"

namespace SailGame { namespace Texas {

using Common::ClientStateMachine;
using Common::Util;
using Dock::IGameAttr;
using ftxui::Element;
using google::protobuf::Any;
using ::Texas::StartGameSettings;

class GameAttr : public IGameAttr {
public:
    virtual std::shared_ptr<ClientStateMachine> GetStateMachine() override {
        return StateMachine::Create();
    }

    virtual std::shared_ptr<Dock::GameScreen> GetGameScreen() override {
        return GameScreen::Create();
    }

    virtual std::shared_ptr<Dock::GameSettingsController>
    GetGameSettingsController() override {
        return std::make_shared<GameSettingsController>();
    }

    virtual Element ShowGameSettings(const Any &any) override {
        auto settings = Util::UnpackGrpcAnyTo<StartGameSettings>(any);
        return Dom::ShowGameSettings(settings);
    }
};

}}  // namespace SailGame::Texas