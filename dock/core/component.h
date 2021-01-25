#pragma once

#include <ftxui/component/component.hpp>

#include "ui_proxy.h"
#include "state.h"

namespace SailGame { namespace Dock {

class SailGameComponent : public ftxui::Component {
public:
    SailGameComponent() = default;

    virtual void TakeFocus() { Component::TakeFocus(); }
};

class ComponentWithUIProxy : public SailGameComponent, public UIProxyClient {};

class DockComponent : public ComponentWithUIProxy {
public:
    DockComponent() = default;

    State GetState() const {
        return dynamic_cast<const State &>(mUIProxy->GetState());
    }
};

}}