#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/component/container.hpp>
#include <ftxui/component/toggle.hpp>
#include <ftxui/screen/string.hpp>
#include <sailgame/uno/card.h>
#include <sailgame_pb/uno/uno.pb.h>
#include <google/protobuf/any.pb.h>

#include "../../../../dock/component/game_settings_ctrl.hpp"
#include "../dom.hpp"

namespace SailGame { namespace Uno {

using namespace ftxui;
using ::Uno::StartGameSettings;
using google::protobuf::Any;

class GameSettingsController : public Dock::GameSettingsController {
public:
    GameSettingsController() {
        mController.Add(&mContainer);
        mContainer.Add(&mIsDraw2ConsumedToggle);
        mContainer.Add(&mCanSkipRespondToggle);
        mContainer.Add(&mHasWildSwapHandsCardToggle);
        mContainer.Add(&mCanDoubtDraw4);
        mContainer.Add(&mRoundTimeToggle);

        // for setting entry that is bool, default toggle (on/off) is enough
        mRoundTimeToggle.entries = {L"5s", L"10s", L"15s"};
    }

    void InitControlMode() override {
        auto settings = GetSettings<StartGameSettings>();
        mIsDraw2ConsumedToggle.selected =
            ConvertBoolToSelected(settings.isdraw2consumed());
        mCanSkipRespondToggle.selected =
            ConvertBoolToSelected(settings.canskiprespond());
        mHasWildSwapHandsCardToggle.selected =
            ConvertBoolToSelected(settings.haswildswaphandscard());
        mCanDoubtDraw4.selected =
            ConvertBoolToSelected(settings.candoubtdraw4());
        mRoundTimeToggle.selected =
            ConvertRoundTimeToSelected(settings.roundtime());
    }

    Element RenderReadOnlyMode() override {
        auto settings = GetSettings<StartGameSettings>();
        return Dom::ShowGameSettings(settings);
    }

    Element RenderControlMode() override {
        return vbox({
            hbox(text(L"isDraw2Consumed       : "), 
                mIsDraw2ConsumedToggle.Render()),
            hbox(text(L"canSkipRespond        : "), 
                mCanSkipRespondToggle.Render()),
            hbox(text(L"hasWildSwapHandsCard  : "), 
                mHasWildSwapHandsCardToggle.Render()),
            hbox(text(L"canDoubtDraw4         : "), 
                mCanDoubtDraw4.Render()),
            hbox(text(L"roundTime             : "), 
                mRoundTimeToggle.Render()),
        });
    }

    Any GetControlResults() override {
        Any results;
        results.PackFrom(MsgBuilder::CreateStartGameSettings(
            ConvertSelectedToBool(mIsDraw2ConsumedToggle.selected),
            ConvertSelectedToBool(mCanSkipRespondToggle.selected),
            ConvertSelectedToBool(mHasWildSwapHandsCardToggle.selected),
            ConvertSelectedToBool(mCanDoubtDraw4.selected),
            ConvertSelectedToRoundTime(mRoundTimeToggle.selected)));
        return results;
    }

private:
    int ConvertRoundTimeToSelected(int roundTime) {
        switch (roundTime) {
            case 5: return 0;
            case 10: return 1;
            case 15: return 2;
            default: throw std::runtime_error("Unsupported round time setting");
        }
    }

    int ConvertSelectedToRoundTime(int selected) {
        switch (selected) {
            case 0: return 5;
            case 1: return 10;
            case 2: return 15;
            default: throw std::runtime_error("selected out of range");
        }
    }

private:
    Container mContainer{Container::Vertical()};
    Toggle mIsDraw2ConsumedToggle;
    Toggle mCanSkipRespondToggle;
    Toggle mHasWildSwapHandsCardToggle;
    Toggle mCanDoubtDraw4;
    Toggle mRoundTimeToggle;
};

}}
