#pragma once

#include <ftxui/component/container.hpp>
#include <ftxui/component/button.hpp>
#include <sailgame/common/core_msg_builder.h>
#include <sailgame_pb/uno/uno.pb.h>

#include "../component.h"
#include "../dom.hpp"

namespace SailGame { namespace Uno {

using namespace ftxui;
using Common::CoreMsgBuilder;
using ::Uno::CardColor;

class SpecifyColorPanel : public UnoComponent {
public:
    std::function<void()> OnCancel;

    std::function<void()> OnNextTurn;

    std::function<void()> OnGameOver;

    SpecifyColorPanel() {
        Add(&mContainer);
        mContainer.Add(&mRedButton);
        mContainer.Add(&mYellowButton);
        mContainer.Add(&mGreenButton);
        mContainer.Add(&mBlueButton);
        mContainer.Add(&mCancelButton);

        mRedButton.on_click = [this] { Specify(CardColor::RED); };
        mYellowButton.on_click = [this] { Specify(CardColor::YELLOW); };
        mGreenButton.on_click = [this] { Specify(CardColor::GREEN); };
        mBlueButton.on_click = [this] { Specify(CardColor::BLUE); };
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

        auto doc = vbox({
            Dom::PlayerBox(to_wstring(username), 
                Dom::ConvertHandcardsToVBox(handcards, mCursor)),
            Dom::TimeIndicator(timeElapsed),
            text(L"Specify the next color."),
            hbox({
                mRedButton.Render(),
                mYellowButton.Render(),
                mGreenButton.Render(),
                mBlueButton.Render(),
                mCancelButton.Render()
            }) | hcenter
        });
        return doc;
    }

    void TakeFocus() override {
        mHasTimeout = false;
        UnoComponent::TakeFocus();
    }

private:
    void Specify(CardColor nextColor) {
        // here the card can be played certainly
        auto cardToPlay = GetState().mSelfState.mHandcards.At(mCursor);
        mUIProxy->OperationInRoom(MsgBuilder::CreatePlay<UserOperation>(
            cardToPlay, nextColor));
    }

    void Timeout() { 
        if (!mHasTimeout) {
            // default to red
            Specify(CardColor::RED);
            mHasTimeout = true;
        }
    }

public:
    int mCursor{0};
    bool mHasTimeout{false};

// private:
public:
    Container mContainer{Container::Horizontal()};
    Button mRedButton{L"Red"};
    Button mYellowButton{L"Yellow"};
    Button mGreenButton{L"Green"};
    Button mBlueButton{L"Blue"};
    Button mCancelButton{L"Cancel"};
};
}}