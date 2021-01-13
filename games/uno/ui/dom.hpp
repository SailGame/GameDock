#pragma once

#include <ftxui/component/container.hpp>
#include <ftxui/screen/string.hpp>
#include <sailgame/uno/card.h>
#include <sailgame/common/util.h>

#include "../src/state.h"

namespace SailGame { namespace Uno {

using namespace ftxui;
using Common::Util;

class Dom {
public:
    static Element PlayerBox(const std::wstring &username, 
        const Element &doc) 
    {
        return vbox({
            text(username) | hcenter,
            separator(),
            doc
        }) | border | hcenter;
    }

    static Element OtherPlayerBox(const std::wstring &username, 
        int cardsRemained, bool doPlayInLastRound, Card lastPlayed) 
    {
        auto lastPlayedText =
            !doPlayInLastRound ? L"" : to_wstring(lastPlayed.ToString());
        return PlayerBox(
            username,
            vbox({
                text(L" cards remained: " + to_wstring(cardsRemained) + L" "),
                text(L" last played: " + lastPlayedText + L" "),
            }));
    }

    static Element OtherPlayerBox(const PlayerState &state) {
        return OtherPlayerBox(to_wstring(state.mUsername), 
            state.mRemainingHandCardsNum, state.mDoPlayInLastRound,
            state.mLastPlayedCard);
    }

    static Element OtherPlayersDoc(const std::vector<PlayerState> &states, 
        int selfIndex) 
    {
        return vbox({
            OtherPlayerBox(states[Util::Wrap(selfIndex + 2, 4)]),
            hbox({
                OtherPlayerBox(states[Util::Wrap(selfIndex + 1, 4)]),
                OtherPlayerBox(states[Util::Wrap(selfIndex - 1, 4)]),
            })
        });
    }

    static Element ConvertCardRowToHBox(const Handcards &handcards, 
        int rowNum, int cursor = -1) 
    {
        auto cardsPerRow = 8;
        auto hBox = hbox({});
        auto start = rowNum * cardsPerRow;
        auto end = std::min(start + cardsPerRow, handcards.Number());
        for (auto i = start; i < end; i++) {
            auto prefix = (i == cursor) ? L">" : L" ";
            hBox->children.push_back(
                /// TODO: attach color
                text(prefix + to_wstring(handcards.At(i).ToString()) + L" "));
        } 
        hBox = hBox | hcenter;
        return hBox;
    }

    static Element ConvertHandcardsToVBox(const Handcards &handcards, 
        int cursor = -1)
    {
        auto cardsPerRow = 8;
        auto vBox = vbox({});
        auto rowNums = handcards.Number() / cardsPerRow + 1;
        for (auto i = 0; i < rowNums; i++) {
            auto hBox = ConvertCardRowToHBox(handcards, i, cursor);
            vBox->children.push_back(hBox);
        }
        auto selfBoxWidth = 42;
        return vBox | size(WIDTH, EQUAL, selfBoxWidth) | hcenter;
    }

};

}}