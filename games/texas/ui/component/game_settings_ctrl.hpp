#pragma once

#include <google/protobuf/any.pb.h>
#include <sailgame/uno/card.h>
#include <sailgame_pb/uno/uno.pb.h>

#include <ftxui/component/component.hpp>
#include <ftxui/screen/string.hpp>

#include "../../../../dock/component/game_settings_ctrl.hpp"
#include "../dom.hpp"

namespace SailGame { namespace Texas {

using namespace ftxui;
using google::protobuf::Any;
using ::Texas::StartGameSettings;

class GameSettingsController : public Dock::GameSettingsController {
public:
    GameSettingsController() {
        mIsDraw2ConsumedToggle =
            Toggle(&bool_toggle_entries, &mIsDraw2ConsumedToggleSelected);
        mCanSkipRespondToggle =
            Toggle(&bool_toggle_entries, &mCanSkipRespondToggleSelected);
        mHasWildSwapHandsCardToggle =
            Toggle(&bool_toggle_entries, &mHasWildSwapHandsCardToggleSelected);
        mCanDoubtDraw4 = Toggle(&bool_toggle_entries, &mCanDoubtDraw4Selected);
        mRoundTimeToggle =
            Toggle(&round_time_toggle_entries, &mRoundTimeToggleSelected);

        mContainer = Container::Vertical(
            {mIsDraw2ConsumedToggle, mCanSkipRespondToggle,
             mHasWildSwapHandsCardToggle, mCanDoubtDraw4, mRoundTimeToggle});
        mController->Add(mContainer);
    }

    void InitControlMode() override {
        auto settings = GetSettings<StartGameSettings>();
        mIsDraw2ConsumedToggleSelected =
            ConvertBoolToSelected(settings.isdraw2consumed());
        mCanSkipRespondToggleSelected =
            ConvertBoolToSelected(settings.canskiprespond());
        mHasWildSwapHandsCardToggleSelected =
            ConvertBoolToSelected(settings.haswildswaphandscard());
        mCanDoubtDraw4Selected =
            ConvertBoolToSelected(settings.candoubtdraw4());
        mRoundTimeToggleSelected =
            ConvertRoundTimeToSelected(settings.roundtime());
    }

    Element RenderReadOnlyMode() override {
        auto settings = GetSettings<StartGameSettings>();
        return Dom::ShowGameSettings(settings);
    }

    Element RenderControlMode() override {
        return vbox({
            hbox(text(L"isDraw2Consumed       : "),
                 mIsDraw2ConsumedToggle->Render()),
            hbox(text(L"canSkipRespond        : "),
                 mCanSkipRespondToggle->Render()),
            hbox(text(L"hasWildSwapHandsCard  : "),
                 mHasWildSwapHandsCardToggle->Render()),
            hbox(text(L"canDoubtDraw4         : "), mCanDoubtDraw4->Render()),
            hbox(text(L"roundTime             : "), mRoundTimeToggle->Render()),
        });
    }

    Any GetControlResults() override {
        Any results;
        results.PackFrom(MsgBuilder::CreateStartGameSettings(
            ConvertSelectedToBool(mIsDraw2ConsumedToggleSelected),
            ConvertSelectedToBool(mCanSkipRespondToggleSelected),
            ConvertSelectedToBool(mHasWildSwapHandsCardToggleSelected),
            ConvertSelectedToBool(mCanDoubtDraw4Selected),
            ConvertSelectedToRoundTime(mRoundTimeToggleSelected)));
        return results;
    }

private:
    int ConvertRoundTimeToSelected(int roundTime) {
        switch (roundTime) {
            case 5:
                return 0;
            case 10:
                return 1;
            case 15:
                return 2;
            default:
                throw std::runtime_error("Unsupported round time setting");
        }
    }

    int ConvertSelectedToRoundTime(int selected) {
        switch (selected) {
            case 0:
                return 5;
            case 1:
                return 10;
            case 2:
                return 15;
            default:
                throw std::runtime_error("selected out of range");
        }
    }

private:
    Component mContainer;
    Component mIsDraw2ConsumedToggle;
    int mIsDraw2ConsumedToggleSelected = 0;
    Component mCanSkipRespondToggle;
    int mCanSkipRespondToggleSelected = 0;
    Component mHasWildSwapHandsCardToggle;
    int mHasWildSwapHandsCardToggleSelected = 0;
    Component mCanDoubtDraw4;
    int mCanDoubtDraw4Selected = 0;
    Component mRoundTimeToggle;
    int mRoundTimeToggleSelected = 0;
};

}}  // namespace SailGame::Texas
