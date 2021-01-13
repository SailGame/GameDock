#pragma once

#include <ftxui/component/container.hpp>
#include <sailgame/uno/card.h>
#include <sailgame/uno/msg_builder.h>
#include <sailgame/common/util.h>
#include <sailgame/common/core_msg_builder.h>
#include "../component/handcards_selector.hpp"
#include "../dom.hpp"
#include "../../../../dock/core/ui_proxy.h"
#include "../../src/state.h"

namespace SailGame { namespace Uno {

using namespace ftxui;
using Common::Util;
using Common::CoreMsgBuilder;

class ChooseCardPanel : public Component, public Dock::UIProxyClient {
public:
    std::function<void()> OnCancel;

    std::function<void()> OnPlay;

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
        mHandcardsSelector.OnMoveLeft = [this] { mCursor--; };
        mHandcardsSelector.OnMoveRight = [this] { mCursor++; };
        mHandcardsSelector.OnPlay = [this] { TryToPlay(); };
        mCancelButton.on_click = [this] { OnCancel(); };
    }

    Element Render() {
        if (!GetState().mGameState.IsMyTurn()) {
            OnPlay();
        }
        auto handcards = GetState().mSelfState.mHandcards;
        auto username = GetState().mPlayerStates[
            GetState().mGameState.mSelfPlayerIndex].mUsername;
        mCursor = Util::Wrap(mCursor, handcards.Number());

        auto doc = vbox({
            Dom::PlayerBox(to_wstring(username), 
                mHandcardsSelector.Render(handcards, mCursor)),
            mCancelButton.Render() | hcenter
        });
        auto selfBoxWidth = 42;
        return doc | size(WIDTH, EQUAL, selfBoxWidth) | hcenter;
    }

    WholeState GetState() const {
        return dynamic_cast<const WholeState &>(mUIProxy->GetState());
    }

    void TryToPlay() {
        auto handcards = GetState().mSelfState.mHandcards;
        auto isUno = (handcards.Number() == 1);
        auto cardToPlay = handcards.At(mCursor);
        if (cardToPlay.CanBePlayedAfter(
            GetState().mGameState.mLastPlayedCard, isUno))
        {
            /// TODO: consider wild card
            mUIProxy->OperationInRoom(
                CoreMsgBuilder::CreateOperationInRoomArgs(
                    MsgBuilder::CreatePlay<UserOperation>(
                        cardToPlay, cardToPlay.mColor)));
        } 
        else {
            // mHandcardsSelector.SetHintText()
        }
    }

public:
    int mCursor{0};

public:
// private:
    Container mContainer{Container::Vertical()};
    HandcardsSelector mHandcardsSelector;
    Button mCancelButton{L"Cancel"};
};
}}