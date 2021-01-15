#pragma once

#include <ftxui/component/container.hpp>
#include <sailgame/uno/card.h>
#include <sailgame/uno/msg_builder.h>
#include <sailgame/common/util.h>
#include <sailgame/common/core_msg_builder.h>

#include "../component/handcards_selector.hpp"
#include "../dom.hpp"
#include "uno_panel.hpp"

namespace SailGame { namespace Uno {

using namespace ftxui;
using Common::Util;
using Common::CoreMsgBuilder;

class ChooseCardPanel : public UnoPanel {
public:
    std::function<void()> OnCancel;

    std::function<void()> OnNextTurn;

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

    Element Render() {
        if (!GetState().mGameState.IsMyTurn()) {
            OnNextTurn();
        }
        auto handcards = GetState().mSelfState.mHandcards;
        auto username = GetState().mPlayerStates[
            GetState().mGameState.mSelfPlayerIndex].mUsername;
        mCursor = Util::Wrap(mCursor, handcards.Number());

        auto doc = vbox({
            Dom::PlayerBox(to_wstring(username), 
                mHandcardsSelector.Render(handcards, mCursor)),
            text(mHintText),
            mCancelButton.Render() | hcenter
        });
        return doc;
    }

    void TakeFocus() override {
        mHintText = L"";
        Component::TakeFocus();
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
                mUIProxy->OperationInRoom(
                    CoreMsgBuilder::CreateOperationInRoomArgs(
                        MsgBuilder::CreatePlay<UserOperation>(
                            cardToPlay, cardToPlay.mColor)));
            }
        } 
        else {
            mHintText = L"You cannot play that card.";
        }
    }

public:
    int mCursor{0};
    std::wstring mHintText;

public:
// private:
    Container mContainer{Container::Vertical()};
    HandcardsSelector mHandcardsSelector;
    Button mCancelButton{L"Cancel"};
};
}}