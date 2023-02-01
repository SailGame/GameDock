#pragma once

#include <ftxui/component/component.hpp>

#include "../component.h"

namespace SailGame { namespace Uno {

using namespace ftxui;

class PlayOrPassPanel : public UnoComponent {
public:
    std::function<void()> OnPlay;

    std::function<void()> OnNextTurn;

    std::function<void()> OnHasChanceToPlayAfterDraw;

    PlayOrPassPanel() {
        mPassButtonLabel = L"Choose a card to play";
        mPlayButton = Button(&mPassButtonLabel, [this] { OnPlay(); });
        mPassButton = Button(L"", [this] { Pass(); });

        mContainer = Container::Horizontal({mPlayButton, mPassButton});
        Add(mContainer);
    }

    void Update() {
        // cannot invoke OnNextTurn in Pass() method because
        // when Pass() is invoked, client hasn't received the
        // broadcast msg that indicates self operation, which means
        // mCurrentPlayer is still himself, if invoking OnNextTurn
        // under such condition, focus will turn to NotMyTurn and then
        // back to PlayOrPass immediately.
        // So invoke OnNextTurn in Render() method.
        if (!GetState().mGameState.IsMyTurn()) {
            OnNextTurn();
        }
        if (mHandcardsNumInLastFrame != -1 &&
            mHandcardsNumInLastFrame <
                GetState().mSelfState.mHandcards.Number()) {
            HandleDrawRsp();
        }
        mHandcardsNumInLastFrame = GetState().mSelfState.mHandcards.Number();
        UpdatePassButtonLabel();
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
        spdlog::info("time elapsed: {}", timeElapsed);

        auto doc = vbox(
            {Dom::PlayerBox(to_wstring(username),
                            Dom::ConvertHandcardsToVBox(handcards)),
             Dom::TimeIndicator(timeElapsed),
             hbox({mPlayButton->Render(), mPassButton->Render()}) | hcenter});
        return doc;
    }

    void TakeFocus() override {
        mHasTimeout = false;
        UnoComponent::TakeFocus();
    }

private:
    void Pass() {
        if (GetState().mGameState.mLastPlayedCard.mText == CardText::SKIP) {
            mUIProxy->OperationInRoom(MsgBuilder::CreateSkip<UserOperation>());
        } else {
            auto number = GetState().mGameState.mCardsNumToDraw;
            mUIProxy->OperationInRoom(
                MsgBuilder::CreateDraw<UserOperation>(number));
        }
    }

    void HandleDrawRsp() {
        if (GetState().mSelfState.mHasChanceToPlayAfterDraw) {
            OnHasChanceToPlayAfterDraw();
        } else {
            mUIProxy->OperationInRoom(MsgBuilder::CreateSkip<UserOperation>());
        }
    }

    void UpdatePassButtonLabel() {
        if (GetState().mGameState.mLastPlayedCard.mText == CardText::SKIP) {
            mPassButtonLabel = L"Skip this turn";
        } else {
            auto number = GetState().mGameState.mCardsNumToDraw;
            mPassButtonLabel = L"Draw " + to_wstring(number) +
                               (number == 1 ? L" card" : L" cards");
        }
    }

    void Timeout() {
        if (!mHasTimeout) {
            Pass();
            mHasTimeout = true;
        }
    }

private:
    // used to detect DrawRsp msg
    int mHandcardsNumInLastFrame{-1};
    bool mHasTimeout{false};

    // private:
public:
    std::wstring mPassButtonLabel;
    Component mContainer;
    Component mPlayButton;
    Component mPassButton;
};
}}  // namespace SailGame::Uno