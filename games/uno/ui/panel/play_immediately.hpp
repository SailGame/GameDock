#pragma once

#include <sailgame/common/core_msg_builder.h>

#include <ftxui/screen/string.hpp>

#include "../component.h"
#include "../dom.hpp"

namespace SailGame { namespace Uno {

using namespace ftxui;
using Common::CoreMsgBuilder;

class PlayImmediatelyPanel : public UnoComponent {
public:
    std::function<void()> OnNextTurn;

    std::function<void()> OnPlayWildCard;

    PlayImmediatelyPanel() {
        mYesButton = Button(L"Yes", [this] { Play(); });
        mNoButton = Button(L"No", [this] { Skip(); });

        mContainer = Container::Horizontal({mYesButton, mNoButton});

        Add(mContainer);
    }

    void Update() {
        if (!GetState().mGameState.IsMyTurn()) {
            OnNextTurn();
        }
        if (GetState().mGameState.mTimeElapsed >= 15) {
            Timeout();
        }
    }

    Element Render() {
        Update();
        auto handcards = GetState().mSelfState.mHandcards;
        auto username =
            GetState()
                .mPlayerStates[GetState().mGameState.mSelfPlayerIndex]
                .mUsername;
        auto cursor = GetState().mSelfState.mIndexOfNewlyDrawn;
        auto timeElapsed = GetState().mGameState.mTimeElapsed;

        auto doc = vbox(
            {Dom::PlayerBox(to_wstring(username),
                            Dom::ConvertHandcardsToVBox(handcards, cursor)),
             Dom::TimeIndicator(timeElapsed), text(mHintText),
             hbox({mYesButton->Render(), mNoButton->Render()}) | hcenter});
        return doc;
    }

    void TakeFocus() override {
        mHintText = L"Whether to play the card just drawn?";
        mHasTimeout = false;
        ComponentBase::TakeFocus();
    }

private:
    void Play() {
        auto index = GetState().mSelfState.mIndexOfNewlyDrawn;
        auto handcards = GetState().mSelfState.mHandcards;
        auto isUno = (handcards.Number() == 1);
        auto cardToPlay = handcards.At(index);
        if (cardToPlay.CanBePlayedAfter(GetState().mGameState.mLastPlayedCard,
                                        isUno)) {
            if (cardToPlay.mColor == CardColor::BLACK) {
                OnPlayWildCard();
            } else {
                mUIProxy->OperationInRoom(MsgBuilder::CreatePlay<UserOperation>(
                    cardToPlay, cardToPlay.mColor));
            }
        } else {
            mHintText = L"You cannot play that card.";
        }
    }

    void Skip() {
        mUIProxy->OperationInRoom(MsgBuilder::CreateSkip<UserOperation>());
    }

    void Timeout() {
        if (!mHasTimeout) {
            Skip();
            mHasTimeout = true;
        }
    }

public:
    std::wstring mHintText;
    bool mHasTimeout{false};

public:
    Component mContainer;
    Component mYesButton;
    Component mNoButton;
};
}}  // namespace SailGame::Uno