#pragma once

#include <ftxui/component/container.hpp>
#include <sailgame/common/types.h>

#include "../../../dock/screen/game_screen.hpp"
#include "../core/state.h"
#include "panel/choose_card.hpp"
#include "panel/play_or_pass.hpp"
#include "panel/not_my_turn.hpp"
#include "panel/play_immediately.hpp"
#include "panel/specify_color.hpp"
#include "panel/game_over.hpp"
#include "dom.hpp"
#include "component.h"

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
        mPanelContainer.Add(&mGameOverPanel);

        mNotMyTurnPanel.OnMyTurn = [this] { mPlayOrPassPanel.TakeFocus(); };
        mNotMyTurnPanel.OnGameOver = [this] { mGameOverPanel.TakeFocus(); };
        mPlayOrPassPanel.OnPlay = [this] { mChooseCardPanel.TakeFocus(); };
        mPlayOrPassPanel.OnNextTurn = [this] { mNotMyTurnPanel.TakeFocus(); };
        mPlayOrPassPanel.OnHasChanceToPlayAfterDraw = [this] { 
            mPlayImmediatelyPanel.TakeFocus();
        };
        mChooseCardPanel.OnCancel = [this] { mPlayOrPassPanel.TakeFocus(); };
        mChooseCardPanel.OnPlayWildCard = [this] {
            mSpecifyColorPanel.mCursor = mChooseCardPanel.mCursor;
            mIsFromChooseCard = true;
            mSpecifyColorPanel.TakeFocus();
        };
        mChooseCardPanel.OnGameOver = [this] { mGameOverPanel.TakeFocus(); };
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
                mChooseCardPanel.TakeFocus(); 
            }
            else {
                mPlayImmediatelyPanel.TakeFocus();
            }
        };
        mSpecifyColorPanel.OnNextTurn = [this] { mNotMyTurnPanel.TakeFocus(); };
        mSpecifyColorPanel.OnGameOver = [this] { mGameOverPanel.TakeFocus(); };
        mGameOverPanel.OnReturn = [this] { OnGameOver(); };
    }

    virtual void SetUIProxy(UIProxy *uiProxy) override {
        UIProxyClient::SetUIProxy(uiProxy);
        mNotMyTurnPanel.SetUIProxy(mUIProxy);
        mPlayOrPassPanel.SetUIProxy(mUIProxy);
        mChooseCardPanel.SetUIProxy(mUIProxy);
        mPlayImmediatelyPanel.SetUIProxy(mUIProxy);
        mSpecifyColorPanel.SetUIProxy(mUIProxy);
        mGameOverPanel.SetUIProxy(mUIProxy);
    }

    static std::shared_ptr<Dock::GameScreen> Create() {
        return std::make_shared<GameScreen>();
    }

    virtual GameType GetGameType() const override { return GameType::Uno; }

    Element Render() override {
        auto selfIndex = GetState().mGameState.mSelfPlayerIndex;
        auto lastPlayedCard = GetState().mGameState.mLastPlayedCard;
        auto doc = vbox({
            Dom::OtherPlayersDoc(
                GetState().mPlayerStates, selfIndex, lastPlayedCard),
            mPanelContainer.Render() | size(WIDTH, EQUAL, 60) | hcenter
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
    GameOverPanel mGameOverPanel;
};
}}