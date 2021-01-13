#pragma once

#include <ftxui/component/container.hpp>
#include <ftxui/screen/string.hpp>

#include "../../../../dock/core/ui_proxy.h"
#include "../../src/state.h"
#include "../dom.hpp"

namespace SailGame { namespace Uno {

using namespace ftxui;

class NotMyTurnPanel : public Component, public Dock::UIProxyClient {
public:
    std::function<void()> OnMyTurn;

    NotMyTurnPanel() = default;

    Element Render() {
        if (GetState().mGameState.IsMyTurn()) {
            OnMyTurn();
        }
        auto handcards = GetState().mSelfState.mHandcards;
        auto username = GetState().mPlayerStates[
            GetState().mGameState.mSelfPlayerIndex].mUsername;

        auto doc = Dom::PlayerBox(to_wstring(username), 
            Dom::ConvertHandcardsToVBox(handcards));
        auto selfBoxWidth = 42;
        return doc | size(WIDTH, EQUAL, selfBoxWidth) | hcenter;
    }

    WholeState GetState() const {
        return dynamic_cast<const WholeState &>(mUIProxy->GetState());
    }
};

}}