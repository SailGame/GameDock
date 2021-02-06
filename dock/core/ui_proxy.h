#pragma once

#include <sailgame/common/core_msg_builder.h>
#include <sailgame/common/game_manager.h>
#include <sailgame/common/network_interface.h>
#include <sailgame/uno/msg_builder.h>
#include <sailgame_pb/core/error.pb.h>
#include <sailgame_pb/core/types.pb.h>
#include <spdlog/spdlog.h>

#include <ftxui/component/component.hpp>
#include <memory>

#include "../util/logger.h"
#include "state_machine.h"

namespace SailGame { namespace Dock {

using Common::ClientGameManager;
using Common::ClientNetworkInterface;
using Common::ClientStateMachine;
using Common::CoreMsgBuilder;
using Common::EventLoop;
using Common::GameType;
using Common::IState;
using namespace ::Core;

class UIProxy {
public:
    std::function<void(GameType)> OnGameStart;

    std::function<void()> OnGameOver;

    std::function<void()> OnExitApp;

    UIProxy(const std::shared_ptr<ClientNetworkInterface> &networkInterface,
            bool isTest = false)
        : mNetworkInterface(networkInterface), mIsTest(isTest) {}

    static std::shared_ptr<UIProxy> Create(
        const std::shared_ptr<ClientNetworkInterface> &networkInterface,
        bool isTest = false) {
        return std::make_shared<UIProxy>(networkInterface, isTest);
    }

    void SwitchToNewStateMachine(
        const std::shared_ptr<ClientStateMachine> &stateMachine) {
        assert(mGameManager);
        mGameManager->SwitchStateMachine(stateMachine);
    }

    void SetGameStartCallback() {
        assert(OnGameStart);
        std::dynamic_pointer_cast<StateMachine>(mGameManager->GetStateMachine())
            ->OnGameStart = [this](GameType game) { OnGameStart(game); };
    }

    void OnLoginSuccess(const std::string &token,
                        const std::string &username = "test") {
        assert(!mGameManager);
        mGameManager = std::make_shared<ClientGameManager>(
            EventLoop::Create(), SailGame::Dock::StateMachine::Create(),
            mNetworkInterface);
        spdlog::info("Game Manager created.");
        State state;
        state.mUsername = username;
        mGameManager->SetState(state);

        SetGameStartCallback();

        // token cannot be captured by reference here
        // because the thread may start running after token gets destructed
        mGameManagerThread = std::make_unique<std::thread>([token, this] {
            if (!mIsTest) {
                mGameManager->StartWithToken(token);
            } else {
                CoreMsgBuilder::SetToken(token);
                // if test, NetworkInterface should Connect instead of
                // AsyncListen
                mGameManager->Start();
            }
        });
        spdlog::info("Game Manager Thread started.");
    }

    void Stop() {
        if (mGameManager) {
            assert(mGameManagerThread);
            mGameManager->Stop();
            mGameManagerThread->join();
        }
    }

    void ExitApp() { OnExitApp(); }

    const IState &GetState() const { return mGameManager->GetState(); };

    void SetState(const IState &state) { mGameManager->SetState(state); }

#define RpcMethod(RpcName, ...)                          \
    auto args = CoreMsgBuilder::Create##RpcName##Args(__VA_ARGS__); \
    Logger::Log(args);                                       \
    auto ret = mNetworkInterface->RpcName(args);             \
    Logger::Log(ret);                                        \
    return ret;

    LoginRet Login(const std::string &username, const std::string &password) {
        RpcMethod(Login, username, password);
    }

    QueryAccountRet QueryAccount(const std::string &username) {
        RpcMethod(QueryAccount, username);
    }

    QueryAccountRet QueryAccount() { RpcMethod(QueryAccount); }

    CreateRoomRet CreateRoom() { RpcMethod(CreateRoom); }

    template <typename GameSettingsT>
    ControlRoomRet ControlRoom(int roomId, const std::string &gameName,
                               const std::string &roomPassword,
                               const GameSettingsT &custom) {
        RpcMethod(ControlRoom, roomId, gameName, roomPassword, custom);
    }

    ListRoomRet ListRoom(const std::string &gameName) {
        RpcMethod(ListRoom, gameName);
    }

    JoinRoomRet JoinRoom(int roomId) { RpcMethod(JoinRoom, roomId); }

    ExitRoomRet ExitRoom() { RpcMethod(ExitRoom); }

    QueryRoomRet QueryRoom(int roomId) { RpcMethod(QueryRoom, roomId); }

    OperationInRoomRet OperationInRoom(Ready ready) {
        RpcMethod(OperationInRoom, ready);
    }

    template <typename UserOperationT>
    OperationInRoomRet OperationInRoom(const UserOperationT &custom) {
        RpcMethod(OperationInRoom, custom);
    }

    MessageRet Message(const std::string &message, const std::string &dstUser,
                       int dstRoom) {
        RpcMethod(Message, message, dstUser, dstRoom);
    }

#undef RpcMethod

    // private:
public:
    std::shared_ptr<ClientNetworkInterface> mNetworkInterface;
    std::shared_ptr<ClientGameManager> mGameManager;
    std::unique_ptr<std::thread> mGameManagerThread;
    bool mIsTest{false};
};

class UIProxyClient {
public:
    virtual void SetUIProxy(UIProxy *uiProxy) { mUIProxy = uiProxy; }

protected:
    UIProxy *mUIProxy;
};

}}  // namespace SailGame::Dock