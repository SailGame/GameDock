#pragma once

#include <ftxui/component/container.hpp>
#include <ftxui/component/button.hpp>
#include <ftxui/screen/string.hpp>
#include <sailgame/common/core_msg_builder.h>

#include "../../../../dock/core/ui_proxy.h"
#include "../../src/state.h"
#include "../dom.hpp"

namespace SailGame { namespace Uno {

using namespace ftxui;
using Common::CoreMsgBuilder;

class PlayImmediatelyPanel : public Component, public Dock::UIProxyClient {
public:
    std::function<void()> OnNextTurn;

    std::function<void()> OnPlayWildCard;

    PlayImmediatelyPanel() {
        Add(&mContainer);
        mContainer.Add(&mYesButton);
        mContainer.Add(&mNoButton);

        mYesButton.on_click = [this] { Play(); };
        mNoButton.on_click = [this] { Skip(); };
    }

    Element Render() {
        if (!GetState().mGameState.IsMyTurn()) {
            OnNextTurn();
        }
        auto handcards = GetState().mSelfState.mHandcards;
        auto username = GetState().mPlayerStates[
            GetState().mGameState.mSelfPlayerIndex].mUsername;
        auto cursor = GetState().mSelfState.mIndexOfNewlyDrawn;

        auto doc = vbox({
            Dom::PlayerBox(to_wstring(username), 
                Dom::ConvertHandcardsToVBox(handcards, cursor)),
            text(L"Whether to play the card just drawn?"),
            hbox({
                mYesButton.Render(),
                mNoButton.Render()
            }) | hcenter
        });
        auto selfBoxWidth = 42;
        return doc | size(WIDTH, EQUAL, selfBoxWidth) | hcenter;
    }

    WholeState GetState() const {
        return dynamic_cast<const WholeState &>(mUIProxy->GetState());
    }

    void Play() {
        auto index = GetState().mSelfState.mIndexOfNewlyDrawn;
        auto handcards = GetState().mSelfState.mHandcards;
        auto isUno = (handcards.Number() == 1);
        auto cardToPlay = handcards.At(index);
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
            // mHandcardsSelector.SetHintText()
        }
    }

    void Skip() {
        mUIProxy->OperationInRoom(CoreMsgBuilder::CreateOperationInRoomArgs(
            MsgBuilder::CreateSkip<UserOperation>()));
    }

public:
    Container mContainer{Container::Horizontal()};
    Button mYesButton{L"Yes"};
    Button mNoButton{L"No"};
};
}}