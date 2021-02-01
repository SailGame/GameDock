#pragma once

#include <ftxui/component/button.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/container.hpp>
#include <ftxui/component/input.hpp>

#include "../component/text_center_button.hpp"
#include "../core/component.h"

namespace SailGame { namespace Dock {

using namespace ftxui;

using ::Core::ErrorNumber;
using ::Core::LoginRet;

class LoginScreen : public DockComponent {
public:
    std::function<void(const LoginRet &)> OnLogin;

    LoginScreen() {
        mOuterContainer.Add(&mContainer);
        mOuterContainer.SetActiveChild(&mContainer);
        mContainer.Add(&mUsernameInput);
        mContainer.Add(&mPasswordInput);
        mContainer.Add(&mLoginButton);

        mUsernameInput.placeholder = L"username";
        mPasswordInput.placeholder = L"password";
        mLoginButton.on_click = [this] {
            auto ret = mUIProxy->Login(to_string(mUsernameInput.content),
                                       to_string(mPasswordInput.content));
            if (ret.err() != ErrorNumber::OK) {
                ShowDialogWithText();
                return;
            }
            OnLogin(ret);
        };
        mDialogOkButton.on_click = [this] {
            mOuterContainer.SetActiveChild(&mContainer);
        };
    }

    Element Render() final {
        auto doc = vbox({
                       text(L"Login") | hcenter,
                       separator(),
                       mUsernameInput.Render(),
                       mPasswordInput.Render(),
                       mLoginButton.Render(),
                   }) |
                   range(40, 10) | center;

        return TryRenderDialog(doc) | range(80, 25) | border | center;
    }

public:
    Container mContainer{Container::Vertical()};
    Input mUsernameInput;
    Input mPasswordInput;
    TextCenterButton mLoginButton{L"Login"};
};

}}  // namespace SailGame::Dock