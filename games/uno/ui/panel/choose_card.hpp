#pragma once

#include <ftxui/component/container.hpp>
#include <sailgame/uno/card.h>
#include <sailgame/common/util.h>
#include "../component/handcards_selector.hpp"
#include "../dom.hpp"
#include "../../../../dock/core/ui_proxy.h"
#include "../../src/state.h"

namespace SailGame { namespace Uno {

using namespace ftxui;
using Common::Util;

class ChooseCardPanel : public Component, public Dock::UIProxyClient {
public:
    std::function<void()> OnCancel;

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
        if (handcards.At(mCursor).CanBePlayedAfter(
            GetState().mGameState.mLastPlayedCard, isUno))
        {
            // mUIProxy->Play
        } 
        else {
            // mHandcardsSelector.SetHintText()
        }
    }

public:
    int mCursor{-1};

private:
    Container mContainer{Container::Vertical()};
    HandcardsSelector mHandcardsSelector;
    Button mCancelButton;
};
}}