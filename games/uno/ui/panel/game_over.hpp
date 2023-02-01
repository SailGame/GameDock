#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/screen/string.hpp>

#include "../component.h"
#include "../dom.hpp"

namespace SailGame { namespace Uno {

using namespace ftxui;

class GameOverPanel : public UnoComponent {
public:
    std::function<void()> OnReturn;

    GameOverPanel() {
        mReturnButton = Button(L"Return", [this] { OnReturn(); });

        mContainer = Container::Horizontal({mReturnButton});

        Add(mContainer);
    }

    Element Render() {
        auto winner = GetState().mGameState.mCurrentPlayer;
        std::string winnerText = "You win!";
        if (winner != GetState().mGameState.mSelfPlayerIndex) {
            winnerText = GetState().mPlayerStates[winner].mUsername + " wins!";
        }

        auto handcards = GetState().mSelfState.mHandcards;
        auto username =
            GetState()
                .mPlayerStates[GetState().mGameState.mSelfPlayerIndex]
                .mUsername;

        auto doc = vbox({Dom::PlayerBox(to_wstring(username),
                                        Dom::ConvertHandcardsToVBox(handcards)),
                         text(to_wstring(winnerText)),
                         mReturnButton->Render() | hcenter});
        return doc;
    }

public:
    Component mContainer;
    Component mReturnButton;
};

}}  // namespace SailGame::Uno