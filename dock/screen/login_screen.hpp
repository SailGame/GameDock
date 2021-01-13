#pragma once

#include <ftxui/component/button.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/input.hpp>
#include <ftxui/component/container.hpp>

#include "../core/ui_proxy.h"

namespace SailGame { namespace Dock {

using namespace ftxui;

using ::Core::ErrorNumber;
using ::Core::LoginRet;

class LoginScreen : public Component, public UIProxyClient {
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
            auto ret = mUIProxy->Login(CoreMsgBuilder::CreateLoginArgs(
                to_string(mUsernameInput.content), 
                to_string(mPasswordInput.content)));
            if (ret.err() == ErrorNumber::OK) {
                OnLogin(ret);
            }
        };
    }

    Element Render() final {
        auto document =
            vbox({
                text(L"Login"),
                separator(),
                mUsernameInput.Render(),
                mPasswordInput.Render(),
                mLoginButton.Render(),
            }) |
            border;

        return document | 
            size(HEIGHT, GREATER_THAN, 18) | 
            size(WIDTH, GREATER_THAN, 18) |
            center;
    }

public:
// private:
    Container mContainer{Container::Vertical()};
    Input mUsernameInput;
    Input mPasswordInput;
    Button mLoginButton{L"Login"};
};

}}