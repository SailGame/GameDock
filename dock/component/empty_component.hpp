#pragma once

#include <ftxui/component/component.hpp>

namespace SailGame { namespace Dock {

using namespace ftxui;

class EmptyComponent : public Component {
public:
    EmptyComponent() = default;

    Element Render() override { return hbox(); }

    bool OnEvent(Event) override { return false; }
};
}}  // namespace SailGame::Dock