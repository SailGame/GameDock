#pragma once

#include <ftxui/component/component.hpp>
#include <functional>

#include "../core/ui_proxy.h"

namespace SailGame { namespace Dock {

using namespace ftxui;

// T is Component and UIProxyClient
template<typename T>
class PolyComponent : public ComponentWithUIProxy {
public:
    ~PolyComponent() override = default;

    bool OnEvent(Event event) override {
        return mComponent ? mComponent->OnEvent(event) : false;
    }

    Element Render() override {
        return mComponent ? mComponent->Render() : hbox();
    }

    Component* ActiveChild() override {
        // active child is the one that is currently held
        return mComponent.get();
    }

    bool HasComponent() const { return mComponent != nullptr; }

    void SetComponent(const std::shared_ptr<T> &component) {
        // append each component ever met as child
        Add(component.get());
        mComponent = component;
        mComponent->SetUIProxy(mUIProxy);
    }

    void ResetComponent() { mComponent.reset(); }

    std::shared_ptr<T> GetComponent() const { return mComponent; }

    template<typename Func, typename... Args>
    auto Invoke(const Func &func, Args &&... args) {
        return std::invoke(func, mComponent, std::forward<Args>(args)...);
    }

protected:
    std::shared_ptr<T> mComponent;
};

}}
