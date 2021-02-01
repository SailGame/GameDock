#include "dom.hpp"

#include "../core/game_attr_fac.h"

namespace ftxui {

Decorator range(int width, int height) {
    return size(WIDTH, EQUAL, width) | size(HEIGHT, EQUAL, height);
}

Decorator width(int value) { return size(WIDTH, EQUAL, value); }

Decorator height(int value) { return size(HEIGHT, EQUAL, value); }

}  // namespace ftxui

namespace SailGame { namespace Dock {

Element Dom::ShowGameSettings(const RoomDetails &details) {
    if (details.gamename().empty()) {
        return text(L"empty");
    }
    return GameAttrFactory::Create(details.gamename())
        ->ShowGameSettings(details.gamesetting());
}

}}  // namespace SailGame::Dock