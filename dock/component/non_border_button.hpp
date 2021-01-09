#pragma once

#include <ftxui/component/button.hpp>

namespace SailGame { namespace Dock {

class NonBorderButton : public ftxui::Button {
public:
    NonBorderButton() = default;

    NonBorderButton(const std::wstring &label) : ftxui::Button(label) {}

    ftxui::Element Render() override {
        if (Focused())
            return ftxui::text(label) | ftxui::inverted;
        else
            return ftxui::text(label);
    }
};
}}