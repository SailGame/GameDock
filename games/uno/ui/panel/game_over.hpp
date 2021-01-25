#pragma once

#include <ftxui/component/container.hpp>
#include <ftxui/component/button.hpp>
#include <ftxui/screen/string.hpp>

#include "../dom.hpp"
#include "../component.h"

namespace SailGame { namespace Uno {

using namespace ftxui;

class GameOverPanel : public UnoComponent {
public:
    std::function<void()> OnReturn;

    GameOverPanel() {
        Add(&mContainer);
        mContainer.Add(&mReturnButton);

        mReturnButton.on_click = [this] { OnReturn(); };
    }

    Element Render() {
        auto winner = GetState().mGameState.mCurrentPlayer;
        std::string winnerText = "You win!";
        if (winner != GetState().mGameState.mSelfPlayerIndex) {
            winnerText = GetState().mPlayerStates[winner].mUsername + " wins!";
        }

        auto handcards = GetState().mSelfState.mHandcards;
        auto username = GetState().mPlayerStates[
            GetState().mGameState.mSelfPlayerIndex].mUsername;

        auto doc = vbox({
            Dom::PlayerBox(to_wstring(username), 
                Dom::ConvertHandcardsToVBox(handcards)),
            text(to_wstring(winnerText)),
            mReturnButton.Render() | hcenter
        });
        return doc;
    }

public:    
    Container mContainer{Container::Horizontal()};
    Button mReturnButton{L"Return"};
};

}}