#pragma once

#include <ftxui/component/container.hpp>

#include "../../../../dock/core/ui_proxy.h"
#include "../../src/state.h"

namespace SailGame { namespace Uno {

using namespace ftxui;

class UnoPanel : public Component, public Dock::UIProxyClient {
public:
    UnoPanel() = default;

    virtual void TakeFocus() { Component::TakeFocus(); }

    WholeState GetState() const {
        return dynamic_cast<const WholeState &>(mUIProxy->GetState());
    }
};

}}