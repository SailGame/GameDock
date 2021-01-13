#pragma once

#include <ftxui/component/container.hpp>
#include <sailgame/common/types.h>
#include "../../../dock/screen/game_screen.hpp"
#include "../src/state.h"
#include "panel/choose_card.hpp"
#include "panel/play_or_pass.hpp"
#include "panel/not_my_turn.hpp"
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

        mNotMyTurnPanel.OnMyTurn = [this] { mPlayOrPassPanel.TakeFocus(); };
        mPlayOrPassPanel.OnPlay = [this] { mChooseCardPanel.TakeFocus(); };
        mPlayOrPassPanel.OnPass = [this] { mNotMyTurnPanel.TakeFocus(); };
        mChooseCardPanel.OnCancel = [this] { mPlayOrPassPanel.TakeFocus(); };
        mChooseCardPanel.OnPlay = [this] { mNotMyTurnPanel.TakeFocus(); };
    }

    virtual void SetUIProxy(UIProxy *uiProxy) override {
        UIProxyClient::SetUIProxy(uiProxy);
        mNotMyTurnPanel.SetUIProxy(mUIProxy);
        mPlayOrPassPanel.SetUIProxy(mUIProxy);
        mChooseCardPanel.SetUIProxy(mUIProxy);
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

public:
    
public:
    Container mPanelContainer{Container::Tab(nullptr)};
    NotMyTurnPanel mNotMyTurnPanel;
    PlayOrPassPanel mPlayOrPassPanel;
    ChooseCardPanel mChooseCardPanel;
};
}}