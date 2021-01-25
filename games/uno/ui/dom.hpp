#pragma once

#include <ftxui/component/container.hpp>
#include <ftxui/screen/string.hpp>
#include <sailgame/uno/card.h>
#include <sailgame/common/util.h>
#include <google/protobuf/any.pb.h>

#include "../core/state.h"

namespace SailGame { namespace Uno {

using namespace ftxui;
using google::protobuf::Any;
using Common::Util;

class Dom {
public:
    static Element TimeIndicator(int current = -1, int total = 15) {
        if (current < 0) {
            return hbox();
        }
        std::wstring indicator = L"[";
        for (auto i = 0; i < total; i++) {
            if (i < current) {
                indicator += L"=";
            }
            else if (i == current) {
                indicator += L">";
            }
            else {
                indicator += L" ";
            }
        }
        indicator += L"]";
        return text(indicator) | hcenter;
    }

    static Element PlayerBox(const std::wstring &username, 
        const Element &doc, int timeElapsed = -1) 
    {
        return vbox({
            vbox({
                text(username) | hcenter,
                separator(),
                doc
            }) | border,
            TimeIndicator(timeElapsed)
        }) | hcenter;
    }

    static Element OtherPlayerBox(const std::wstring &username, 
        int cardsRemained, bool doPlayInLastRound, Card lastPlayed, 
        int timeElapsed = -1) 
    {
        auto lastPlayedText =
            !doPlayInLastRound ? L"" : to_wstring(lastPlayed.ToString());
        return PlayerBox(
            username,
            vbox({
                text(L" cards remained: " + to_wstring(cardsRemained) + L" "),
                text(L" last played: " + lastPlayedText + L" "),
            }), timeElapsed);
    }

    static Element OtherPlayerBox(const PlayerState &state, int timeElapsed = -1) {
        return OtherPlayerBox(to_wstring(state.mUsername), 
            state.mRemainingHandCardsNum, state.mDoPlayInLastRound,
            state.mLastPlayedCard, timeElapsed);
    }

    static Element OtherPlayersDoc(const std::vector<PlayerState> &states, 
        int selfIndex, Card lastPlayedCard, int curPlayer, int timeElapsed) 
    {
        // assume there are only 2 players
        if (curPlayer == selfIndex) {
            return vbox({
                OtherPlayerBox(states[Util::Wrap(selfIndex - 1, 2)]),
                ConvertCardToFtxText(lastPlayedCard) | hcenter
            });
        }
        return vbox({
            OtherPlayerBox(states[Util::Wrap(selfIndex - 1, 2)], timeElapsed),
            ConvertCardToFtxText(lastPlayedCard) | hcenter
        });

        /// TODO: support multi-player
        // return vbox({
        //     OtherPlayerBox(states[Util::Wrap(selfIndex + 2, 4)]),
        //     hbox({
        //         OtherPlayerBox(states[Util::Wrap(selfIndex + 1, 4)]),
        //         OtherPlayerBox(states[Util::Wrap(selfIndex - 1, 4)]),
        //     })
        // });
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
            hBox->children.push_back(text(prefix));
            hBox->children.push_back(ConvertCardToFtxText(handcards.At(i)));
            hBox->children.push_back(text(L" "));
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

    static Element ConvertCardToFtxText(Card card) {
        auto color = Color::Red;
        switch (card.mColor) {
            case CardColor::RED: color = Color::Red; break;
            case CardColor::YELLOW: color = Color::Yellow; break;
            case CardColor::GREEN: color = Color::Green; break;
            case CardColor::BLUE: color = Color::Blue; break;
            default: return text(to_wstring(card.ToString()));
        }
        return text(to_wstring(card.ToString())) | ftxui::color(color);
    }

    static Element ShowGameSettings(const StartGameSettings &settings) {
        auto vBox = vbox({});
        auto boolToWstring = [](bool b) -> std::wstring {
            return b ? L"true" : L"false"; 
        };
        vBox->children.push_back(text(L"isDraw2Consumed       : " + 
            boolToWstring(settings.isdraw2consumed())));
        vBox->children.push_back(text(L"canSkipRespond        : " + 
            boolToWstring(settings.canskiprespond())));
        vBox->children.push_back(text(L"hasWildSwapHandsCard  : " + 
            boolToWstring(settings.haswildswaphandscard())));
        vBox->children.push_back(text(L"canDoubtDraw4         : " + 
            boolToWstring(settings.candoubtdraw4())));
        vBox->children.push_back(text(L"roundTime             : " + 
            to_wstring(settings.roundtime())));
        return vBox;
    }
};

}}