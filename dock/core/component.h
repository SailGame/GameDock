#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/component/container.hpp>
#include <ftxui/component/button.hpp>
#include <ftxui/screen/string.hpp>

#include "state.h"
#include "ui_proxy.h"
#include "../util/dom.hpp"

namespace SailGame { namespace Dock {

using namespace ftxui;

class SailGameComponent : public ftxui::Component {
public:
    SailGameComponent() = default;

    virtual void TakeFocus() { Component::TakeFocus(); }
};

class ComponentWithUIProxy : public SailGameComponent, public UIProxyClient {};

class DockComponent : public ComponentWithUIProxy {
public:
    DockComponent() {
        Add(&mOuterContainer);
        mOuterContainer.Add(&mDialogOkButton);
    }

    State GetState() const {
        return dynamic_cast<const State &>(mUIProxy->GetState());
    }

    void ShowDialogWithText(const std::string &text = "Unknown Error.") {
        mDialogText = text;
        mOuterContainer.SetActiveChild(&mDialogOkButton);
    }

    Element TryRenderDialog(const Element &doc) {
        if (mDialogOkButton.Active()) {
            return dbox({
                doc,
                vbox({
                    text(L"Error") | center,
                    separator(), text(L""), text(L""),
                    text(to_wstring(mDialogText)) | center, text(L""), text(L""),
                    mDialogOkButton.Render() | center
                }) | range(40, 10) | border | clear_under | center 
            });            
        }
        return doc;
    }

protected:
    std::string mDialogText;

protected:
    Container mOuterContainer{Container::Tab(nullptr)};
    Button mDialogOkButton{L"   Ok   "};
};

}}  // namespace SailGame::Dock