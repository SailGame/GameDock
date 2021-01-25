#pragma once

#include <ftxui/component/container.hpp>

#include "../../../dock/core/component.h"
#include "../../../dock/core/ui_proxy.h"
#include "../core/state.h"

namespace SailGame { namespace Uno {

using namespace ftxui;

class UnoComponent : public Dock::ComponentWithUIProxy {
public:
    UnoComponent() = default;

    WholeState GetState() const {
        return dynamic_cast<const WholeState &>(mUIProxy->GetState());
    }
};

}}  // namespace SailGame::Uno