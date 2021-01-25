#pragma once

#include <ftxui/component/container.hpp>
#include <ftxui/screen/string.hpp>

#include "../dom.hpp"
#include "../component.h"

namespace SailGame { namespace Uno {

using namespace ftxui;

class NotMyTurnPanel : public UnoComponent {
public:
    std::function<void()> OnMyTurn;

    std::function<void()> OnGameOver;

    NotMyTurnPanel() = default;

    void Update() {
        if (GetState().mGameState.mGameEnds) {
            OnGameOver();
        }
        if (GetState().mGameState.IsMyTurn()) {
            OnMyTurn();
        }
    }

    Element Render() {
        Update();
        auto handcards = GetState().mSelfState.mHandcards;
        auto username = GetState().mPlayerStates[
            GetState().mGameState.mSelfPlayerIndex].mUsername;

        auto doc = Dom::PlayerBox(to_wstring(username), 
            Dom::ConvertHandcardsToVBox(handcards));
        return doc;
    }
};

}}