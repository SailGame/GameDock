#pragma once

#include <sailgame/common/core_msg_builder.h>
#include <sailgame_pb/uno/uno.pb.h>

#include <ftxui/component/component.hpp>

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
        mRedButton = Button(
            L"Red", [this] { Specify(CardColor::RED); },
            ButtonOption::Animated(ftxui::Color::Red));
        mYellowButton = Button(
            L"Yellow", [this] { Specify(CardColor::YELLOW); },
            ButtonOption::Animated(ftxui::Color::Yellow));
        mGreenButton = Button(
            L"Green", [this] { Specify(CardColor::GREEN); },
            ButtonOption::Animated(ftxui::Color::Green));
        mBlueButton = Button(
            L"Blue", [this] { Specify(CardColor::BLUE); },
            ButtonOption::Animated(ftxui::Color::Blue));
        mCancelButton = Button(L"Cancel", [this] { OnCancel(); });

        mContainer =
            Container::Horizontal({mRedButton, mYellowButton, mGreenButton,
                                   mBlueButton, mCancelButton});
        Add(mContainer);
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
        auto username =
            GetState()
                .mPlayerStates[GetState().mGameState.mSelfPlayerIndex]
                .mUsername;
        auto timeElapsed = GetState().mGameState.mTimeElapsed;

        auto doc = vbox(
            {Dom::PlayerBox(to_wstring(username),
                            Dom::ConvertHandcardsToVBox(handcards, mCursor)),
             Dom::TimeIndicator(timeElapsed), text(L"Specify the next color."),
             hbox({mRedButton->Render(), mYellowButton->Render(),
                   mGreenButton->Render(), mBlueButton->Render(),
                   mCancelButton->Render()}) |
                 hcenter});
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
        mUIProxy->OperationInRoom(
            MsgBuilder::CreatePlay<UserOperation>(cardToPlay, nextColor));
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
    Component mContainer;
    Component mRedButton;
    Component mYellowButton;
    Component mGreenButton;
    Component mBlueButton;
    Component mCancelButton;
};
}}  // namespace SailGame::Uno