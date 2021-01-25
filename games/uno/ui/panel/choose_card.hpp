#pragma once

#include <ftxui/component/container.hpp>
#include <ftxui/component/button.hpp>
#include <sailgame/uno/card.h>
#include <sailgame/uno/msg_builder.h>
#include <sailgame/common/util.h>
#include <sailgame/common/core_msg_builder.h>

#include "../component/handcards_selector.hpp"
#include "../dom.hpp"
#include "../component.h"

namespace SailGame { namespace Uno {

using namespace ftxui;
using Common::Util;
using Common::CoreMsgBuilder;

class ChooseCardPanel : public UnoComponent {
public:
    std::function<void()> OnCancel;

    std::function<void()> OnNextTurn;

    std::function<void()> OnGameOver;

    std::function<void()> OnPlayWildCard;

    ChooseCardPanel() {
        Add(&mContainer);
        mContainer.Add(&mHandcardsSelector);
        mContainer.Add(&mCancelButton);

        // wrap mCursor in Render method instead of here.
        // ftxui processes all pending events before rendering next frame
        // so if OnMove and OnPlay event come and get processed together,
        // actually it's possible that a wrong card (out of range)
        // will be played because we havn't wrap it in Render method.
        // but such possibility is too small, and considering that
        // we want event callbacks defined in constructor, mCursor is
        // not wrapped here.
        mHandcardsSelector.OnMoveLeft = [this] {
            mHintText = L"";
            mCursor--;
        };
        mHandcardsSelector.OnMoveRight = [this] { 
            mHintText = L"";
            mCursor++; 
        };
        mHandcardsSelector.OnPlay = [this] { 
            mHintText = L"";
            TryToPlay(); 
        };
        mCancelButton.on_click = [this] { OnCancel(); };
    }

    void Update() {
        if (GetState().mGameState.mGameEnds) {
            assert(GetState().mSelfState.mHandcards.Number() == 0);
            assert(GetState().mGameState.mCurrentPlayer ==
                   GetState().mGameState.mSelfPlayerIndex);
            OnGameOver();
        }
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
        auto username = GetState().mPlayerStates[
            GetState().mGameState.mSelfPlayerIndex].mUsername;
        auto timeElapsed = GetState().mGameState.mTimeElapsed;
        mCursor = (handcards.Number() == 0 ? -1
            : Util::Wrap(mCursor, handcards.Number()));

        auto doc = vbox({
            Dom::PlayerBox(to_wstring(username), 
                mHandcardsSelector.Render(handcards, mCursor)),
            Dom::TimeIndicator(timeElapsed),
            text(mHintText),
            mCancelButton.Render() | hcenter
        });
        return doc;
    }

    void TakeFocus() override {
        mHintText = L"";
        mHandcardsSelector.TakeFocus();
        mHasTimeout = false;
        // Component::TakeFocus();
    }

private:
    void TryToPlay() {
        auto handcards = GetState().mSelfState.mHandcards;
        auto isUno = (handcards.Number() == 1);
        auto cardToPlay = handcards.At(mCursor);
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

    void Timeout() { 
        if (!mHasTimeout) {
            OnCancel();
            mHasTimeout = true;
        }
    }

public:
    int mCursor{0};
    std::wstring mHintText;
    // if don't use this flag, there may be a potential repetitive timeout:
    // because we change focus after receiving msg from core,
    // if before msg comes, refresher posts an event again
    // Timeout method will be invoked twice.
    bool mHasTimeout{false};

public:
// private:
    Container mContainer{Container::Vertical()};
    HandcardsSelector mHandcardsSelector;
    Button mCancelButton{L"Cancel"};
};
}}