#pragma once

#include <sailgame/common/types.h>

#include <ftxui/component/container.hpp>

#include "../../../dock/screen/game_screen.hpp"
#include "../../../dock/util/dom.hpp"
#include "../core/state.h"
#include "component.h"
#include "dom.hpp"
#include "panel/choose_card.hpp"
#include "panel/game_over.hpp"
#include "panel/not_my_turn.hpp"
#include "panel/play_immediately.hpp"
#include "panel/play_or_pass.hpp"
#include "panel/specify_color.hpp"

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
            } else {
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
        auto curPlayer = GetState().mGameState.mCurrentPlayer;
        auto timeElapsed = GetState().mGameState.mTimeElapsed;
        auto doc =
            vbox({Dom::OtherPlayersDoc(GetState().mPlayerStates, selfIndex,
                                       lastPlayedCard, curPlayer, timeElapsed),
                  mPanelContainer.Render() | width(60) | hcenter});
        return doc | center | range(80, 25) | border;
    }

    WholeState GetState() const {
        return dynamic_cast<const WholeState &>(mUIProxy->GetState());
    }

    bool OnEvent(Event event) override {
        if (event == Event::Custom) {
            mTick++;
            auto ticksPerSecond = static_cast<int>(1 / 0.05);
            if (mTick == ticksPerSecond) {
                mTick = 0;
                auto state = GetState();
                /// FIXME: concurrency issue
                state.mGameState.mTimeElapsed++;
                mUIProxy->SetState(state);
            }
            return true;
        }
        return Dock::GameScreen::OnEvent(event);
    }

private:
    // indicate whether SpecifyColorPanel is switched from ChooseCardPanel
    // or PlayImmediatelyPanel, to return correctly after cancel.
    bool mIsFromChooseCard;
    int mTick{0};

public:
    Container mPanelContainer{Container::Tab(nullptr)};
    NotMyTurnPanel mNotMyTurnPanel;
    PlayOrPassPanel mPlayOrPassPanel;
    ChooseCardPanel mChooseCardPanel;
    PlayImmediatelyPanel mPlayImmediatelyPanel;
    SpecifyColorPanel mSpecifyColorPanel;
    GameOverPanel mGameOverPanel;
};
}}  // namespace SailGame::Uno