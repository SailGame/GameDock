#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/screen/string.hpp>
#include <iostream>

#include "../util/dom.hpp"
#include "state.h"
#include "ui_proxy.h"

namespace SailGame { namespace Dock {

using namespace ftxui;

class SailGameComponent : public ftxui::ComponentBase {
public:
    SailGameComponent() = default;

    virtual void TakeFocus() { ComponentBase::TakeFocus(); }
};

class ComponentWithUIProxy : public SailGameComponent, public UIProxyClient {};

class DockComponent : public ComponentWithUIProxy {
public:
    DockComponent() {
        mContainer = Container::Vertical({});
        mExitYesButton = Button(L"  Yes  ", [this] { mUIProxy->ExitApp(); });
        mExitNoButton = Button(L"  No  ", [this] {
            mOuterContainer->SetActiveChild(mContainer.get());
        });

        mExitContainer = Container::Horizontal({
            mExitYesButton,
            mExitNoButton,
        });

        mDialogOkButton = Button(L"  Ok  ", [this] {
            mOuterContainer->SetActiveChild(mContainer.get());
        });

        mOuterContainerSelected = 0;
        mOuterContainer =
            Container::Tab({mContainer, mDialogOkButton, mExitContainer},
                           &mOuterContainerSelected);

        Add(mOuterContainer);
    }

    State GetState() const {
        return dynamic_cast<const State &>(mUIProxy->GetState());
    }

    void ShowDialogWithText(const std::string &text = "Unknown Error.") {
        mDialogText = text;
        mOuterContainer->SetActiveChild(mDialogOkButton.get());
    }

    virtual bool OnEvent(Event event) override {
        if (event == Event::Escape) {
            mOuterContainer->SetActiveChild(mExitContainer.get());
            return true;
        }
        return ComponentWithUIProxy::OnEvent(event);
    }

    Element TryRenderDialog(const Element &doc) {
        if (mDialogOkButton->Active()) {
            return dbox(
                {doc, vbox({text(L"Error") | center, separator(), text(L""),
                            text(L""), text(to_wstring(mDialogText)) | center,
                            text(L""), text(L""),
                            mDialogOkButton->Render() | center}) |
                          range(40, 10) | border | clear_under | center});
        }
        if (mExitContainer->Active()) {
            return dbox(
                {doc, vbox({
                          text(L"Exit") | center,
                          separator(),
                          text(L""),
                          text(L""),
                          text(L"Exit SailGame?") | center,
                          text(L""),
                          text(L""),
                          hbox({filler(), mExitYesButton->Render(), filler(),
                                mExitNoButton->Render(), filler()}),
                      }) | range(40, 10) |
                          border | clear_under | center});
        }
        return doc;
    }

protected:
    std::string mDialogText;

protected:
    int mOuterContainerSelected;
    Component mOuterContainer;
    Component mContainer;
    Component mDialogOkButton;
    Component mExitContainer;
    Component mExitYesButton;
    Component mExitNoButton;
};

}}  // namespace SailGame::Dock