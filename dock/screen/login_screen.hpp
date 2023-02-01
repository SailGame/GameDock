#pragma once

#include <ftxui/component/component.hpp>

#include "../core/component.h"

namespace SailGame { namespace Dock {

using namespace ftxui;

using ::Core::ErrorNumber;
using ::Core::LoginRet;

class LoginScreen : public DockComponent {
public:
    std::function<void(const LoginRet &)> OnLogin;

    LoginScreen() {
        mUsernameInput = Input(&mUsernameInputContent, L"username");
        mPasswordInput = Input(&mPasswordInputContent, L"password");
        mLoginButton = Button(L"Login", [this] {
            auto ret =
                mUIProxy->Login(mUsernameInputContent, mPasswordInputContent);
            if (ret.err() != ErrorNumber::OK) {
                ShowDialogWithText(ErrorNumber_Name(ret.err()));
                return;
            }
            OnLogin(ret);
        });

        mContainer->Add(mUsernameInput);
        mContainer->Add(mPasswordInput);
        mContainer->Add(mLoginButton);
    }

    Element Render() final {
        auto doc = vbox({
                       text(L"Login") | hcenter,
                       separator(),
                       mUsernameInput->Render(),
                       mPasswordInput->Render(),
                       mLoginButton->Render(),
                   }) |
                   range(40, 10) | center;

        return TryRenderDialog(doc) | range(80, 25) | border | center;
    }

public:
    std::string mUsernameInputContent;
    std::string mPasswordInputContent;
    Component mUsernameInput;
    Component mPasswordInput;
    Component mLoginButton;
};

}}  // namespace SailGame::Dock