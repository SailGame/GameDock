#include <sailgame_pb/core/types.pb.h>

#include <ftxui/screen/string.hpp>

#include "../matcher.h"
#include "../screen_fixture.h"

namespace SailGame { namespace Test {

using namespace testing;
using namespace Dock;
using Common::CoreMsgBuilder;
using ::Core::ErrorNumber;
using ftxui::to_string;
using ftxui::to_wstring;
using grpc::Status;

class LoginScreenFixture : public ScreenFixture {
public:
    LoginScreenFixture() = default;
};

TEST_F(LoginScreenFixture, LoginSuccess) {
    std::string username = "tbc";
    std::string password = "123456";
    std::string token = "tttoken";
    auto points = 21;
    mDock.mLoginScreen->mUsernameInputContent = username;
    mDock.mLoginScreen->mPasswordInputContent = password;

    auto loginRet = CoreMsgBuilder::CreateLoginRet(
        ErrorNumber::OK, token,
        CoreMsgBuilder::CreateAccount(username, points));
    EXPECT_CALL(*mMockStub, Login(_, LoginArgsMatcher(username, password), _))
        .Times(1)
        .WillOnce(DoAll(SetArgPointee<2>(loginRet), Return(Status::OK)));
    EXPECT_CALL(*mMockStub, ListenRaw(_, ListenArgsMatcher(token)))
        .Times(1)
        .WillOnce(Return(mMockStream));
    mDock.mLoginScreen->mLoginButton->TakeFocus();
    mDock.mLoginScreen->mLoginButton->OnEvent(Event::Return);
    EXPECT_EQ(mDock.mLobbyScreen->mUsername, username);
    EXPECT_EQ(mDock.mLobbyScreen->mPoints, points);
    EXPECT_TRUE(mDock.mLobbyScreen->Active());
}

TEST_F(LoginScreenFixture, LoginFailure) {
    std::string username = "tbc";
    std::string password = "errorpassword";
    mDock.mLoginScreen->mUsernameInputContent = username;
    mDock.mLoginScreen->mPasswordInputContent = password;

    // what token and account will return if error number is not ok?
    auto loginRet = CoreMsgBuilder::CreateLoginRet(
        ErrorNumber::UnkownError, "", CoreMsgBuilder::CreateAccount("", 0));
    EXPECT_CALL(*mMockStub, Login(_, LoginArgsMatcher(username, password), _))
        .Times(1)
        .WillOnce(DoAll(SetArgPointee<2>(loginRet), Return(Status::OK)));
    EXPECT_CALL(*mMockStub, ListenRaw(_, _)).Times(0);
    mDock.mLoginScreen->mLoginButton->TakeFocus();
    mDock.mLoginScreen->mLoginButton->OnEvent(Event::Return);
    EXPECT_TRUE(mDock.mLoginScreen->Active());
    EXPECT_FALSE(mDock.mLobbyScreen->Active());
}
}}  // namespace SailGame::Test