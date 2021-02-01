#pragma once

#include <ftxui/component/button.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/container.hpp>
#include <ftxui/screen/string.hpp>
#include <iostream>

#include "../util/dom.hpp"
#include "state.h"
#include "ui_proxy.h"

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
        mOuterContainer.Add(&mContainer);
        mOuterContainer.Add(&mDialogOkButton);
        mOuterContainer.Add(&mExitContainer);

        mExitContainer.Add(&mExitYesButton);
        mExitContainer.Add(&mExitNoButton);

        mDialogOkButton.on_click = [this] {
            mOuterContainer.SetActiveChild(&mContainer);
        };

        mExitYesButton.on_click = [this] { mUIProxy->ExitApp(); };

        mExitNoButton.on_click = [this] {
            mOuterContainer.SetActiveChild(&mContainer);
        };
    }

    State GetState() const {
        return dynamic_cast<const State &>(mUIProxy->GetState());
    }

    void ShowDialogWithText(const std::string &text = "Unknown Error.") {
        mDialogText = text;
        mOuterContainer.SetActiveChild(&mDialogOkButton);
    }

    virtual bool OnEvent(Event event) override {
        if (event == Event::Escape) {
            mOuterContainer.SetActiveChild(&mExitContainer);
            return true;
        }
        return ComponentWithUIProxy::OnEvent(event);
    }

    Element TryRenderDialog(const Element &doc) {
        if (mDialogOkButton.Active()) {
            return dbox(
                {doc, vbox({text(L"Error") | center, separator(), text(L""),
                            text(L""), text(to_wstring(mDialogText)) | center,
                            text(L""), text(L""),
                            mDialogOkButton.Render() | center}) |
                          range(40, 10) | border | clear_under | center});
        }
        if (mExitContainer.Active()) {
            return dbox(
                {doc, vbox({
                          text(L"Exit") | center,
                          separator(),
                          text(L""),
                          text(L""),
                          text(L"Exit SailGame?") | center,
                          text(L""),
                          text(L""),
                          hbox({filler(), mExitYesButton.Render(), filler(),
                                mExitNoButton.Render(), filler()}),
                      }) | range(40, 10) |
                          border | clear_under | center});
        }
        return doc;
    }

protected:
    std::string mDialogText;

protected:
    Container mOuterContainer{Container::Tab(nullptr)};
    Container mContainer{Container::Vertical()};
    Button mDialogOkButton{L"   Ok   "};
    Container mExitContainer{Container::Horizontal()};
    Button mExitYesButton{L"  Yes  "};
    Button mExitNoButton{L"  No  "};
};

}}  // namespace SailGame::Dock