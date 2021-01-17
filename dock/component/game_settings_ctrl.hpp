#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/component/container.hpp>
#include <google/protobuf/any.pb.h>
#include <sailgame/common/util.h>
#include <sailgame/common/types.h>

#include "../core/ui_proxy.h"
#include "empty_component.hpp"

namespace SailGame { namespace Dock {

using namespace ftxui;
using Common::Util;
using Common::GameType;
using google::protobuf::Any;

class GameSettingsController : public ComponentWithUIProxy {
public:
    GameSettingsController() {
        Add(&mTabContainer);
        mTabContainer.Add(&mEmptyComponent);
        mTabContainer.Add(&mController);
    }

    void ControlMode() {
        InitControlMode();
        mController.TakeFocus();
    }

    void ReadOnlyMode() { mTabContainer.SetActiveChild(&mEmptyComponent); }

    virtual Any GetControlResults() = 0;

    virtual Element Render() {
        if (mUIProxy->mGameManager->GetGameType() != GameType::NoGame) {
            return hbox();
        }
        return mController.Active() ? RenderControlMode()
                                    : RenderReadOnlyMode();
    }

protected:
    virtual void InitControlMode() = 0;

    virtual Element RenderReadOnlyMode() = 0;

    virtual Element RenderControlMode() = 0;

    template<typename T>
    T GetSettings() {
        auto settings = dynamic_cast<const Dock::State &>(mUIProxy->GetState())
            .mRoomDetails.gamesetting();
        return Util::UnpackGrpcAnyTo<T>(settings);
    }

    int ConvertBoolToSelected(bool setting) {
        // on: 0, off: 1
        return (setting ? 0 : 1);
    }

    bool ConvertSelectedToBool(int selected) { return (selected == 0); }

protected:
    Container mTabContainer{Container::Tab(nullptr)};
    EmptyComponent mEmptyComponent;
    Component mController;
};
}}