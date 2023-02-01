#pragma once

#include <ftxui/component/component_options.hpp>

namespace SailGame { namespace Dock {

using namespace ftxui;

inline ButtonOption NonBorderButtonOption() {
    ButtonOption option;
    option.transform = [](const EntryState& s) {
        auto element = text(s.label);
        if (s.focused) {
            element |= inverted;
        }
        return element;
    };
    return option;
}

static ButtonOption TextCenterButtonOption() {
    ButtonOption option;
    option.transform = [](const EntryState& s) {
        auto element = text(s.label) | hcenter | border;
        if (s.focused) {
            element |= inverted;
        }
        return element;
    };
    return option;
};

}}  // namespace SailGame::Dock