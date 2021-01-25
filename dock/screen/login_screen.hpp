#pragma once

#include <ftxui/component/button.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/input.hpp>
#include <ftxui/component/container.hpp>

#include "../core/component.h"
#include "../component/text_center_button.hpp"

namespace SailGame { namespace Dock {

using namespace ftxui;

using ::Core::ErrorNumber;
using ::Core::LoginRet;

class LoginScreen : public DockComponent {
public:
    std::function<void(const LoginRet &)> OnLogin;

    LoginScreen()
    {
        Add(&mContainer);
        mContainer.Add(&mUsernameInput);
        mContainer.Add(&mPasswordInput);
        mContainer.Add(&mLoginButton);

        mUsernameInput.placeholder = L"username";
        mPasswordInput.placeholder = L"password";
        mLoginButton.on_click = [this] {
            auto ret = mUIProxy->Login(to_string(mUsernameInput.content), 
                to_string(mPasswordInput.content));
            if (ret.err() == ErrorNumber::OK) {
                OnLogin(ret);
            }
        };
    }

    Element Render() final {
        auto doc = vbox({
            text(L"Login") | hcenter,
            separator(),
            mUsernameInput.Render(),
            mPasswordInput.Render(),
            mLoginButton.Render(),
        }) | range(40, 10) | center;

        return doc | range(80, 25) | border | center;
    }

public:
    Container mContainer{Container::Vertical()};
    Input mUsernameInput;
    Input mPasswordInput;
    TextCenterButton mLoginButton{L"Login"};
};

}}