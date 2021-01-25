#pragma once

#include <sailgame/uno/card.h>

#include <ftxui/component/component.hpp>
#include <ftxui/screen/string.hpp>

#include "../dom.hpp"

namespace SailGame { namespace Uno {

using namespace ftxui;

class HandcardsSelector : public Component {
public:
    std::function<void()> OnMoveLeft;

    std::function<void()> OnMoveRight;

    std::function<void()> OnPlay;

    HandcardsSelector() = default;

    Element Render(const Handcards &handcards, int cursor) {
        if (!Focused()) {
            // hide cursor if not focused
            cursor = -1;
        }

        return Dom::ConvertHandcardsToVBox(handcards, cursor);
    }

    bool OnEvent(Event event) override {
        if (event == Event::ArrowLeft) {
            OnMoveLeft();
            return true;
        }
        if (event == Event::ArrowRight) {
            OnMoveRight();
            return true;
        }
        if (event == Event::Return || event == Event::Character(' ')) {
            OnPlay();
            return true;
        }
        return false;
    }

private:
    int mCardsPerRow{8};
};

}}  // namespace SailGame::Uno
