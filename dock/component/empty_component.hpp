#pragma once

#include <ftxui/component/component_base.hpp>

namespace SailGame { namespace Dock {

using namespace ftxui;

class EmptyComponent : public ComponentBase {
public:
    EmptyComponent() = default;

    Element Render() override { return hbox(); }

    bool OnEvent(Event) override { return false; }
};
}}  // namespace SailGame::Dock