#pragma once

#include <google/protobuf/any.pb.h>
#include <sailgame/common/util.h>
#include <sailgame/uno/card.h>

#include <ftxui/component/container.hpp>
#include <ftxui/screen/string.hpp>

#include "../core/state.h"

namespace SailGame { namespace Uno {

using namespace ftxui;
using Common::Util;
using google::protobuf::Any;

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
            } else if (i == current) {
                indicator += L">";
            } else {
                indicator += L" ";
            }
        }
        indicator += L"]";
        return text(indicator) | hcenter;
    }

    static Element PlayerBox(const std::wstring &username, const Element &doc,
                             int timeElapsed = -1) {
        return vbox(
                   {vbox({text(username) | hcenter, separator(), doc}) | border,
                    TimeIndicator(timeElapsed)}) |
               hcenter;
    }

    static Element OtherPlayerBox(const std::wstring &username,
                                  int cardsRemained, bool doPlayInLastRound,
                                  Card lastPlayed, int timeElapsed = -1) {
        auto lastPlayedText =
            !doPlayInLastRound ? L"" : to_wstring(lastPlayed.ToString());
        return PlayerBox(
            username,
            vbox({
                text(L" cards remained: " + to_wstring(cardsRemained) + L" "),
                text(L" last played: " + lastPlayedText + L" "),
            }),
            timeElapsed);
    }

    static Element OtherPlayerBox(const PlayerState &state,
                                  int timeElapsed = -1) {
        return OtherPlayerBox(
            to_wstring(state.mUsername), state.mRemainingHandCardsNum,
            state.mDoPlayInLastRound, state.mLastPlayedCard, timeElapsed);
    }

    static Element OtherPlayersDoc(const std::vector<PlayerState> &states,
                                   int selfIndex, Card lastPlayedCard,
                                   int curPlayer, int timeElapsed) {
        if (curPlayer == selfIndex) {
            timeElapsed = -1;
        }
        if (states.size() == 2) {
            return OtherPlayersDoc_2(states, selfIndex, lastPlayedCard,
                                     curPlayer, timeElapsed);
        }
        if (states.size() == 3) {
            return OtherPlayersDoc_3(states, selfIndex, lastPlayedCard,
                                     curPlayer, timeElapsed);
        }
        if (states.size() == 4) {
            return OtherPlayersDoc_4(states, selfIndex, lastPlayedCard,
                                     curPlayer, timeElapsed);
        }
        throw std::runtime_error("Unsupported player num");
    }

    static Element OtherPlayersDoc_2(const std::vector<PlayerState> &states,
                                     int selfIndex, Card lastPlayedCard,
                                     int curPlayer, int timeElapsed) {
        return vbox(
            {OtherPlayerBox(states[Util::Wrap(selfIndex - 1, 2)], timeElapsed),
             ConvertCardToFtxText(lastPlayedCard) | hcenter});
    }

    static Element OtherPlayersDoc_3(const std::vector<PlayerState> &states,
                                     int selfIndex, Card lastPlayedCard,
                                     int curPlayer, int timeElapsed) {
        return vbox({hbox({OtherPlayerBox(states[Util::Wrap(selfIndex + 1, 3)],
                                          timeElapsed),
                           OtherPlayerBox(states[Util::Wrap(selfIndex - 1, 3)],
                                          timeElapsed)}),
                     ConvertCardToFtxText(lastPlayedCard) | hcenter});
    }

    static Element OtherPlayersDoc_4(const std::vector<PlayerState> &states,
                                     int selfIndex, Card lastPlayedCard,
                                     int curPlayer, int timeElapsed) {
        return vbox({OtherPlayerBox(states[Util::Wrap(selfIndex + 2, 4)]),
                     text(L""),
                     hbox({
                         OtherPlayerBox(states[Util::Wrap(selfIndex + 1, 4)]),
                         vbox({
                             ConvertCardToFtxText(lastPlayedCard) | center,
                         }) | yflex,
                         OtherPlayerBox(states[Util::Wrap(selfIndex - 1, 4)]),
                     }),
                     text(L"")});
    }

    static Element ConvertCardRowToHBox(const Handcards &handcards, int rowNum,
                                        int cursor = -1) {
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
                                          int cursor = -1) {
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

    static ftxui::Color ConvertCardColorToFtxColor(CardColor color) {
        switch (color) {
            case CardColor::RED:
                return ftxui::Color::Red;
            case CardColor::YELLOW:
                return ftxui::Color::Yellow;
            case CardColor::GREEN:
                return ftxui::Color::Green;
            case CardColor::BLUE:
                return ftxui::Color::Blue;
            default:
                return ftxui::Color::Black;
        }
    }

    static Element ConvertCardToFtxText(Card card) {
        if (card.mColor == CardColor::BLACK) {
            // if wild card, show as black in light terminal
            // and show as white in dark terminal
            return text(to_wstring(card.ToString()));
        }
        auto color = ConvertCardColorToFtxColor(card.mColor);
        return text(to_wstring(card.ToString())) | ftxui::color(color);
    }

    static Element ShowGameSettings(const StartGameSettings &settings) {
        auto vBox = vbox({});
        auto boolToWstring = [](bool b) -> std::wstring {
            return b ? L"true" : L"false";
        };
        vBox->children.push_back(
            text(L"isDraw2Consumed       : " +
                 boolToWstring(settings.isdraw2consumed())));
        vBox->children.push_back(
            text(L"canSkipRespond        : " +
                 boolToWstring(settings.canskiprespond())));
        vBox->children.push_back(
            text(L"hasWildSwapHandsCard  : " +
                 boolToWstring(settings.haswildswaphandscard())));
        vBox->children.push_back(text(L"canDoubtDraw4         : " +
                                      boolToWstring(settings.candoubtdraw4())));
        vBox->children.push_back(text(L"roundTime             : " +
                                      to_wstring(settings.roundtime())));
        return vBox;
    }
};

}}  // namespace SailGame::Uno