#pragma once

#include <ftxui/component/button.hpp>
#include <ftxui/screen/color.hpp>

namespace SailGame { namespace Uno {

using namespace ftxui;

class ColorfulButton : public ftxui::Button {
public:
    ColorfulButton() = default;

    ColorfulButton(const std::wstring &label, ftxui::Color color) 
        : ftxui::Button(label) , mColor(color) {}

    ftxui::Element Render() override {
        if (Focused())
            return text(label) | border | inverted | color(mColor);
        else
            return text(label) | border | color(mColor);
    }

private:
    ftxui::Color mColor;
};
}}  // namespace SailGame::Dock