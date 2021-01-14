#pragma once

#include <ftxui/component/container.hpp>
#include <sailgame/common/types.h>
#include "../../../dock/screen/game_screen.hpp"
#include "../src/state.h"
#include "panel/choose_card.hpp"
#include "panel/play_or_pass.hpp"
#include "panel/not_my_turn.hpp"
#include "panel/play_immediately.hpp"
#include "panel/specify_color.hpp"
#include "dom.hpp"

namespace SailGame { namespace Uno {

using namespace ftxui;
using Common::GameType;
using Dock::UIProxy;
using Dock::UIProxyClient;

class GameScreen : public Dock::GameScreen {
public:
    GameScreen() {
        Add(&mPanelContainer);
        mPanelContainer.Add(&mNotMyTurnPanel);
        mPanelContainer.Add(&mPlayOrPassPanel);
        mPanelContainer.Add(&mChooseCardPanel);
        mPanelContainer.Add(&mPlayImmediatelyPanel);
        mPanelContainer.Add(&mSpecifyColorPanel);

        mNotMyTurnPanel.OnMyTurn = [this] { mPlayOrPassPanel.TakeFocus(); };
        mPlayOrPassPanel.OnPlay = [this] {
            /// TODO: these panels could do some initialization work 
            /// before taking focus. should factor this logic out.
            mChooseCardPanel.mHintText = L"";
            mChooseCardPanel.TakeFocus();
        };
        mPlayOrPassPanel.OnNextTurn = [this] { mNotMyTurnPanel.TakeFocus(); };
        mPlayOrPassPanel.OnHasChanceToPlayAfterDraw = [this] {
            mPlayImmediatelyPanel.mHintText =
                L"Whether to play the card just drawn?";
            mPlayImmediatelyPanel.TakeFocus();
        };
        mChooseCardPanel.OnCancel = [this] { mPlayOrPassPanel.TakeFocus(); };
        mChooseCardPanel.OnPlayWildCard = [this] {
            mSpecifyColorPanel.mCursor = mChooseCardPanel.mCursor;
            mIsFromChooseCard = true;
            mSpecifyColorPanel.TakeFocus();
        };
        mChooseCardPanel.OnNextTurn = [this] { mNotMyTurnPanel.TakeFocus(); };
        mPlayImmediatelyPanel.OnNextTurn = [this] {
            mNotMyTurnPanel.TakeFocus();
        };
        mPlayImmediatelyPanel.OnPlayWildCard = [this] {
            mSpecifyColorPanel.mCursor =
                GetState().mSelfState.mIndexOfNewlyDrawn;
            mIsFromChooseCard = false;
            mSpecifyColorPanel.TakeFocus();
        };
        mSpecifyColorPanel.OnCancel = [this] {
            if (mIsFromChooseCard) {
                mChooseCardPanel.mHintText = L"";
                mChooseCardPanel.TakeFocus();
            }
            else {
                mPlayImmediatelyPanel.mHintText =
                    L"Whether to play the card just drawn?";
                mPlayImmediatelyPanel.TakeFocus();
            }
        };
        mSpecifyColorPanel.OnNextTurn = [this] { mNotMyTurnPanel.TakeFocus(); };
    }

    virtual void SetUIProxy(UIProxy *uiProxy) override {
        UIProxyClient::SetUIProxy(uiProxy);
        mNotMyTurnPanel.SetUIProxy(mUIProxy);
        mPlayOrPassPanel.SetUIProxy(mUIProxy);
        mChooseCardPanel.SetUIProxy(mUIProxy);
        mPlayImmediatelyPanel.SetUIProxy(mUIProxy);
        mSpecifyColorPanel.SetUIProxy(mUIProxy);
    }

    static std::shared_ptr<Dock::GameScreen> Create() {
        return std::make_shared<GameScreen>();
    }

    virtual GameType GetGameType() const override { return GameType::Uno; }

    Element Render() {
        auto selfIndex = GetState().mGameState.mSelfPlayerIndex;
        auto doc = vbox({
            Dom::OtherPlayersDoc(GetState().mPlayerStates, selfIndex),
            mPanelContainer.Render()
        });
        return doc | size(WIDTH, EQUAL, 80) | border | center;
    }

    WholeState GetState() const {
        return dynamic_cast<const WholeState &>(mUIProxy->GetState());
    }

private:
    // indicate whether SpecifyColorPanel is switched from ChooseCardPanel
    // or PlayImmediatelyPanel, to return correctly after cancel.
    bool mIsFromChooseCard;

public:
    Container mPanelContainer{Container::Tab(nullptr)};
    NotMyTurnPanel mNotMyTurnPanel;
    PlayOrPassPanel mPlayOrPassPanel;
    ChooseCardPanel mChooseCardPanel;
    PlayImmediatelyPanel mPlayImmediatelyPanel;
    SpecifyColorPanel mSpecifyColorPanel;
};
}}