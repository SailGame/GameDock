#pragma once

#include <ftxui/component/container.hpp>
#include <ftxui/screen/string.hpp>

#include "../../../../dock/core/ui_proxy.h"
#include "../../src/state.h"
#include "../dom.hpp"
#include "uno_panel.hpp"

namespace SailGame { namespace Uno {

using namespace ftxui;

class NotMyTurnPanel : public UnoPanel {
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
        return doc;
    }
};

}}