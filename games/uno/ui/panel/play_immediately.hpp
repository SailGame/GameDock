#pragma once

#include <ftxui/component/container.hpp>
#include <ftxui/component/button.hpp>
#include <ftxui/screen/string.hpp>
#include <sailgame/common/core_msg_builder.h>

#include "../dom.hpp"
#include "../component.h"

namespace SailGame { namespace Uno {

using namespace ftxui;
using Common::CoreMsgBuilder;

class PlayImmediatelyPanel : public UnoComponent {
public:
    std::function<void()> OnNextTurn;

    std::function<void()> OnPlayWildCard;

    PlayImmediatelyPanel() {
        Add(&mContainer);
        mContainer.Add(&mYesButton);
        mContainer.Add(&mNoButton);

        mYesButton.on_click = [this] { Play(); };
        mNoButton.on_click = [this] { Skip(); };
    }

    void Update() {
        if (!GetState().mGameState.IsMyTurn()) {
            OnNextTurn();
        }
    }

    Element Render() {
        Update();
        auto handcards = GetState().mSelfState.mHandcards;
        auto username = GetState().mPlayerStates[
            GetState().mGameState.mSelfPlayerIndex].mUsername;
        auto cursor = GetState().mSelfState.mIndexOfNewlyDrawn;

        auto doc = vbox({
            Dom::PlayerBox(to_wstring(username), 
                Dom::ConvertHandcardsToVBox(handcards, cursor)),
            text(mHintText),
            hbox({
                mYesButton.Render(),
                mNoButton.Render()
            }) | hcenter
        });
        return doc;
    }

    void TakeFocus() override {
        mHintText = L"Whether to play the card just drawn?";
        Component::TakeFocus();
    }
    
private:
    void Play() {
        auto index = GetState().mSelfState.mIndexOfNewlyDrawn;
        auto handcards = GetState().mSelfState.mHandcards;
        auto isUno = (handcards.Number() == 1);
        auto cardToPlay = handcards.At(index);
        if (cardToPlay.CanBePlayedAfter(
            GetState().mGameState.mLastPlayedCard, isUno))
        {
            if (cardToPlay.mColor == CardColor::BLACK) {
                OnPlayWildCard();
            }
            else {
                mUIProxy->OperationInRoom(MsgBuilder::CreatePlay<UserOperation>(
                    cardToPlay, cardToPlay.mColor));
            }
        } 
        else {
            mHintText = L"You cannot play that card.";
        }
    }

    void Skip() {
        mUIProxy->OperationInRoom(MsgBuilder::CreateSkip<UserOperation>());
    }

public:
    std::wstring mHintText;

public:
    Container mContainer{Container::Horizontal()};
    Button mYesButton{L"Yes"};
    Button mNoButton{L"No"};
};
}}