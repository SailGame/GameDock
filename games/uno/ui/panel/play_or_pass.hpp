#pragma once

#include <ftxui/component/container.hpp>
#include <ftxui/component/button.hpp>

#include "uno_panel.hpp"

namespace SailGame { namespace Uno {

using namespace ftxui;

class PlayOrPassPanel : public UnoPanel {
public:
    std::function<void()> OnPlay;

    std::function<void()> OnNextTurn;

    std::function<void()> OnHasChanceToPlayAfterDraw;

    PlayOrPassPanel() {
        Add(&mContainer);
        mContainer.Add(&mPlayButton);
        mContainer.Add(&mPassButton);

        mPlayButton.on_click = [this] { OnPlay(); };
        mPassButton.on_click = [this] { Pass(); };
    }

    Element Render() {
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
        if (mHandcardsNumInLastFrame < 
            GetState().mSelfState.mHandcards.Number()) 
        {
            HandleDrawRsp();
        }
        mHandcardsNumInLastFrame = GetState().mSelfState.mHandcards.Number();
        UpdatePassButtonLabel();
        auto handcards = GetState().mSelfState.mHandcards;
        auto username = GetState().mPlayerStates[
            GetState().mGameState.mSelfPlayerIndex].mUsername;

        auto doc = vbox({
            Dom::PlayerBox(to_wstring(username), 
                Dom::ConvertHandcardsToVBox(handcards)),
            hbox({
                mPlayButton.Render(),
                mPassButton.Render()
            }) | hcenter
        });
        return doc;
    }

private:
    void Pass() {
        if (GetState().mGameState.mLastPlayedCard.mText == CardText::SKIP) {
            mUIProxy->OperationInRoom(
                CoreMsgBuilder::CreateOperationInRoomArgs(
                    MsgBuilder::CreateSkip<UserOperation>()));
        }
        else {
            auto number = GetState().mGameState.mCardsNumToDraw;
            mUIProxy->OperationInRoom(
                CoreMsgBuilder::CreateOperationInRoomArgs(
                    MsgBuilder::CreateDraw<UserOperation>(number)));
        }
    }

    void HandleDrawRsp() {
        if (GetState().mSelfState.mHasChanceToPlayAfterDraw) {
            OnHasChanceToPlayAfterDraw();
        }
        else {
            mUIProxy->OperationInRoom(
                CoreMsgBuilder::CreateOperationInRoomArgs(
                    MsgBuilder::CreateSkip<UserOperation>()));
        }
    }

    void UpdatePassButtonLabel() {
        if (GetState().mGameState.mLastPlayedCard.mText == CardText::SKIP) {
            mPassButton.label = L"Skip this turn";
        }
        else {
            auto number = GetState().mGameState.mCardsNumToDraw;
            mPassButton.label = L"Draw " + to_wstring(number)
                + (number == 1 ? L" card" : L"cards");
        }
    }

private:
    // used to detect DrawRsp msg
    int mHandcardsNumInLastFrame;

// private:
public:
    Container mContainer{Container::Horizontal()};
    Button mPlayButton{L"Choose a card to play"};
    Button mPassButton;
};
}}