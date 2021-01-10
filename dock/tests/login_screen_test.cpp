#include <gtest/gtest.h>
#include <grpcpp/test/mock_stream.h>

#include <sailgame_pb/core/types.pb.h>
#include <sailgame_pb/core/core_mock.grpc.pb.h>
#include <ftxui/screen/string.hpp>

#include "../dock.h"
#include "matcher.h"

namespace SailGame { namespace Test {

using namespace testing;
using namespace Dock;
using Common::CoreMsgBuilder;
using Common::NetworkInterface;
using ::Core::BroadcastMsg;
using ::Core::ErrorNumber;
using ::Core::MockGameCoreStub;
using ftxui::to_string;
using ftxui::to_wstring;
using grpc::Status;
using grpc::testing::MockClientReader;

class LoginScreenFixture : public ::testing::Test {
public:
    LoginScreenFixture() 
        : mMockStream(new MockClientReader<BroadcastMsg>()),
        mMockStub(std::make_shared<MockGameCoreStub>()),
        mDock(UIProxy::Create(
            NetworkInterface<false>::Create(mMockStub), true))
    {}

    void SetUp() {
        spdlog::set_level(spdlog::level::err);
        mThread = std::make_unique<std::thread>([&] {
            mDock.Loop();
        });
        using namespace std::chrono_literals;
        // make sure mDock has entered loop before running test case
        std::this_thread::sleep_for(10ms);
    }

    void TearDown() {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1s);
        mDock.mScreen.ExitLoopClosure()();
        mDock.mUIProxy->Stop();
        mThread->join();
    }

    void UserEvent(const std::function<void()> &callback) {
        // wait for 1 seconds to display ui
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1s);
        callback();
        // an event is needed to refresh ui, i don't know why
        mDock.mScreen.PostEvent(ftxui::Event::Custom);
    }

protected:
    MockClientReader<BroadcastMsg> *mMockStream;
    std::shared_ptr<MockGameCoreStub> mMockStub;

    Dock::Dock mDock;
    std::unique_ptr<std::thread> mThread;
};

TEST_F(LoginScreenFixture, LoginSuccess) {
    std::string username = "tbc";
    std::string password = "123456";
    std::string token = "tttoken";
    auto points = 21;
    mDock.mLoginScreen.mUsernameInput.content = to_wstring(username);
    mDock.mLoginScreen.mPasswordInput.content = to_wstring(password);
    UserEvent([]{});
    auto loginRet = CoreMsgBuilder::CreateLoginRet(ErrorNumber::OK, token, 
        CoreMsgBuilder::CreateAccount(username, points));
    EXPECT_CALL(*mMockStub, Login(_, LoginArgsMatcher(username, password), _))
        .Times(1)
        .WillOnce(DoAll(SetArgPointee<2>(loginRet), Return(Status::OK)));
    EXPECT_CALL(*mMockStub, ListenRaw(_, ListenArgsMatcher(token)))
        .Times(1)
        .WillOnce(Return(mMockStream));
    mDock.mLoginScreen.mLoginButton.TakeFocus();
    UserEvent(mDock.mLoginScreen.mLoginButton.on_click);
    EXPECT_EQ(mDock.mLobbyScreen.mUsername, username);
    EXPECT_EQ(mDock.mLobbyScreen.mPoints, points);
    EXPECT_TRUE(mDock.mLobbyScreen.Active());
}

TEST_F(LoginScreenFixture, LoginFailure) {
    std::string username = "tbc";
    std::string password = "errorpassword";
    mDock.mLoginScreen.mUsernameInput.content = to_wstring(username);
    mDock.mLoginScreen.mPasswordInput.content = to_wstring(password);
    UserEvent([]{});
    // what token and account will return if error number is not ok?
    auto loginRet = CoreMsgBuilder::CreateLoginRet(ErrorNumber::UnkownError, 
        "", CoreMsgBuilder::CreateAccount("", 0));
    EXPECT_CALL(*mMockStub, Login(_, LoginArgsMatcher(username, password), _))
        .Times(1)
        .WillOnce(DoAll(SetArgPointee<2>(loginRet), Return(Status::OK)));
    EXPECT_CALL(*mMockStub, ListenRaw(_, _)).Times(0);
    mDock.mLoginScreen.mLoginButton.TakeFocus();
    UserEvent(mDock.mLoginScreen.mLoginButton.on_click);
    EXPECT_TRUE(mDock.mLoginScreen.Active());
    EXPECT_FALSE(mDock.mLobbyScreen.Active());
}
}}