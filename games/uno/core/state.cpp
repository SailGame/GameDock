#include "state.h"

namespace SailGame { namespace Uno {

void GameState::UpdateAfterDraw()
{
    if (CardSet::DrawTexts.count(mLastPlayedCard.mText)) {
        // last played card will become EMPTY after the draw penalty is consumed
        mLastPlayedCard.mText = CardText::EMPTY;
    }
    // the number of cards to draw falls back to 1
    mCardsNumToDraw = 1;

    // no need to invoke NextPlayer() here 
    // because a draw action is always followed by a skip or play action
}

void GameState::UpdateAfterSkip()
{
    if (mLastPlayedCard.mText == CardText::SKIP) {
        // last played card will become EMPTY after the skip penalty is consumed
        mLastPlayedCard.mText = CardText::EMPTY;
    }
    NextPlayer();
}

void GameState::UpdateAfterPlay(Card card)
{
    if (card.mText == CardText::WILD) {
        // if just a common wild card (not +4), don't affect the number text
        mLastPlayedCard.mColor = card.mColor;
    }
    else {
        mLastPlayedCard = card;
    }
    
    if (card.mText == CardText::REVERSE) {
        mIsInClockwise = !mIsInClockwise;
    }
    if (card.mText == CardText::DRAW_TWO) {
        // in the normal state, mCardsNumToDraw is equal to 1
        // once a player plays a `Draw` card, the effect is gonna accumulate
        mCardsNumToDraw = (mCardsNumToDraw == 1) ? 2 : (mCardsNumToDraw + 2);
    }
    if (card.mText == CardText::DRAW_FOUR) {
        mCardsNumToDraw = (mCardsNumToDraw == 1) ? 4 : (mCardsNumToDraw + 4);
    }

    NextPlayer();
}

void GameState::NextPlayer()
{
    mCurrentPlayer = mIsInClockwise ? 
        Util::Wrap(mCurrentPlayer + 1, mPlayerNum) :
        Util::Wrap(mCurrentPlayer - 1, mPlayerNum);
    // mTimeElapsed = 0;
}

void SelfState::UpdateAfterDrawRsp(const std::vector<Card> &cards)
{
    Handcards originalHandCards = mHandcards;
    mHandcards.Draw(cards);
    if (cards.size() == 1) {
        mHasChanceToPlayAfterDraw = true;
        mIndexOfNewlyDrawn = mHandcards.GetIndexOfNewlyDrawn(originalHandCards);
    }
}

void SelfState::UpdateAfterSkip()
{
    mHasChanceToPlayAfterDraw = false;
}

void SelfState::UpdateAfterPlay(Card card)
{
    mHandcards.Erase(card);
    mHasChanceToPlayAfterDraw = false;
}

void PlayerState::UpdateAfterDraw(int number)
{
    mRemainingHandCardsNum += number;
    mDoPlayInLastRound = false;
}

void PlayerState::UpdateAfterSkip()
{
    mDoPlayInLastRound = false;
}

void PlayerState::UpdateAfterPlay(Card card)
{
    mRemainingHandCardsNum--;
    mDoPlayInLastRound = true;
    mLastPlayedCard = card;
}


}}