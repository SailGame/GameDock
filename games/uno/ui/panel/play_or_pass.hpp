#pragma once

#include <ftxui/component/container.hpp>
#include <ftxui/component/button.hpp>

#include "../../../../dock/core/ui_proxy.h"
#include "../../src/state.h"

namespace SailGame { namespace Uno {

using namespace ftxui;

class PlayOrPassPanel : public Component, public Dock::UIProxyClient {
public:
    std::function<void()> OnPlay;

    PlayOrPassPanel() {
        Add(&mContainer);
        mContainer.Add(&mPlayButton);
        mContainer.Add(&mPassButton);

        mPlayButton.on_click = [this] { OnPlay(); };
        mPassButton.on_click = [this] { Pass(); };
    }

    Element Render() {
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
        auto selfBoxWidth = 42;
        return doc | size(WIDTH, EQUAL, selfBoxWidth) | hcenter;
    }

    WholeState GetState() const {
        return dynamic_cast<const WholeState &>(mUIProxy->GetState());
    }

    void Pass() {
        // when Pass is invoked, mIsSkip and mCardsNumToDraw have been assigned
        if (mIsSkip) {
            // uiproxy->skip
        }
        else {
            // uiproxy->draw(cardnum)
        }
    }

    void UpdatePassButtonLabel() {
        mIsSkip = 
            (GetState().mGameState.mLastPlayedCard.mText == CardText::SKIP);
        if (mIsSkip) {
            mPassButton.label = L"Skip this turn";
        }
        else {
            mCardsNumToDraw = GetState().mGameState.mCardsNumToDraw;
            mPassButton.label = L"Draw " + to_wstring(mCardsNumToDraw)
                + (mCardsNumToDraw == 1 ? L" card" : L"cards");
        }
    }

private:
    bool mIsSkip;
    int mCardsNumToDraw;

private:
    Container mContainer{Container::Horizontal()};
    Button mPlayButton{L"Choose a card to play"};
    Button mPassButton;
};
}}