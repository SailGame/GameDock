#pragma once

#include <map>
#include <vector>

#include <sailgame_pb/uno/uno.pb.h>
#include <sailgame_pb/core/types.pb.h>

#include <sailgame/common/util.h>
#include <sailgame/common/state_machine.h>
#include <sailgame/uno/card.h>
// #include "util.h"

namespace SailGame { namespace Uno {

using Common::IState;
using Common::Util;
using ::Uno::StartGameSettings;

class GameState {
public:
    GameState() = default;

    void UpdateAfterDraw();
    
    void UpdateAfterSkip();
    
    void UpdateAfterPlay(Card card);

    void NextPlayer();

    bool IsMyTurn() const { 
        return mCurrentPlayer == mSelfPlayerIndex && 
            mSelfPlayerIndex != -1; 
    }

public:
    int mPlayerNum;
    StartGameSettings mGameSettings;
    int mSelfPlayerIndex{-1};
    
    int mCurrentPlayer{-1};
    bool mIsInClockwise{true};
    Card mLastPlayedCard{};
    int mCardsNumToDraw{1};  // +2 and +4 can accumulate
    int mTimeElapsed{0};
};

class SelfState {
public:
    SelfState() = default;

    void UpdateAfterDrawRsp(const std::vector<Card> &cards);
    
    void UpdateAfterSkip();
    
    void UpdateAfterPlay(Card card);

public:
    Handcards mHandcards;
    bool mHasChanceToPlayAfterDraw{false};
    int mIndexOfNewlyDrawn{-1};
};

class PlayerState {
public:
    PlayerState() = default;

    PlayerState(const std::string &username) : mUsername(username) {}

    void UpdateAfterDraw(int number);
    
    void UpdateAfterSkip();
    
    void UpdateAfterPlay(Card card);

public:
    std::string mUsername;
    int mRemainingHandCardsNum{-1};
    bool mDoPlayInLastRound{false};
    Card mLastPlayedCard{};
};

class WholeState : public IState {
public:
    // WholeState(const BasicState &basicState) : BasicState(basicState) {}
    // ~WholeState() = default;

public:
    GameState mGameState;
    SelfState mSelfState;
    std::vector<PlayerState> mPlayerStates;  // use playerIndex as index
};
}}