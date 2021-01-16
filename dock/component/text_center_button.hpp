#pragma once

#include <ftxui/component/button.hpp>

namespace SailGame { namespace Dock {

using namespace ftxui;

class TextCenterButton : public Button {
public:
    TextCenterButton() = default;

    TextCenterButton(const std::wstring &label) : Button(label) {}

    Element Render() override {
        if (Focused())
            return text(label) | hcenter | border | inverted;
        else
            return text(label) | hcenter | border;
    }
};
}}