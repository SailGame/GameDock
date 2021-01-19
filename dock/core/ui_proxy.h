#pragma once

#include <memory>
#include <spdlog/spdlog.h>
#include <ftxui/component/component.hpp>
#include <sailgame/common/game_manager.h>
#include <sailgame/common/network_interface.h>
#include <sailgame/common/core_msg_builder.h>
#include <sailgame_pb/core/types.pb.h>
#include <sailgame_pb/core/error.pb.h>

#include <sailgame/uno/msg_builder.h>

#include "state_machine.h"
#include "../util/logger.h"

namespace SailGame { namespace Dock {

using Common::CoreMsgBuilder;
using Common::EventLoop;
using Common::ClientGameManager;
using Common::ClientNetworkInterface;
using Common::IState;
using Common::ClientStateMachine;
using namespace ::Core;

class UIProxy {
public:
    UIProxy(const std::shared_ptr<ClientNetworkInterface> &networkInterface,
        bool isTest = false)
        : mNetworkInterface(networkInterface), mIsTest(isTest) {}
    
    static std::shared_ptr<UIProxy> Create(
        const std::shared_ptr<ClientNetworkInterface> &networkInterface,
        bool isTest = false) 
    {
        return std::make_shared<UIProxy>(networkInterface, isTest);
    }

    void SwitchToNewStateMachine(
        const std::shared_ptr<ClientStateMachine> &stateMachine)
    {
        assert(mGameManager);
        mGameManager->SwitchStateMachine(stateMachine);
    }

    void OnLoginSuccess(const std::string &token, 
        const std::string &username = "test") 
    {
        assert(!mGameManager);
        mGameManager = std::make_shared<ClientGameManager>(
            EventLoop::Create(), 
            SailGame::Dock::StateMachine::Create(username), mNetworkInterface);
        spdlog::info("Game Manager created.");
        // token cannot be captured by reference here
        // because the thread may start running after token gets destructed
        mGameManagerThread = std::make_unique<std::thread>([token, this] {
            if (!mIsTest) {
                mGameManager->StartWithToken(token);
            }
            else {
                CoreMsgBuilder::SetToken(token);
                // if test, NetworkInterface should Connect instead of AsyncListen
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
    
    const IState &GetState() const { return mGameManager->GetState(); };

    void SetState(const IState &state) { mGameManager->SetState(state); }

#define RpcMethodHelper(RpcName) \
    Logger::Log(args); \
    auto ret = mNetworkInterface->RpcName(args); \
    Logger::Log(ret); \
    return ret;

#define RpcMethod(RpcName) \
    auto args = CoreMsgBuilder::Create##RpcName##Args(); \
    RpcMethodHelper(RpcName);

#define RpcMethod_P1(RpcName, p1) \
    auto args = CoreMsgBuilder::Create##RpcName##Args(p1); \
    RpcMethodHelper(RpcName);

#define RpcMethod_P2(RpcName, p1, p2) \
    auto args = CoreMsgBuilder::Create##RpcName##Args(p1, p2); \
    RpcMethodHelper(RpcName);

#define RpcMethod_P3(RpcName, p1, p2, p3) \
    auto args = CoreMsgBuilder::Create##RpcName##Args(p1, p2, p3); \
    RpcMethodHelper(RpcName);

#define RpcMethod_P4(RpcName, p1, p2, p3, p4) \
    auto args = CoreMsgBuilder::Create##RpcName##Args(p1, p2, p3, p4); \
    RpcMethodHelper(RpcName);

    LoginRet Login(const std::string &username, const std::string &password) {
        RpcMethod_P2(Login, username, password);
    }

    QueryAccountRet QueryAccount(const std::string &username) {
        RpcMethod_P1(QueryAccount, username);
    }

    QueryAccountRet QueryAccount() {
        RpcMethod(QueryAccount);
    }

    CreateRoomRet CreateRoom() {
        RpcMethod(CreateRoom);
    }

    template<typename GameSettingsT>
    ControlRoomRet ControlRoom(int roomId, 
        const std::string &gameName, const std::string &roomPassword,
        const GameSettingsT &custom) {
        RpcMethod_P4(ControlRoom, roomId, gameName, roomPassword, custom);
    }

    ListRoomRet ListRoom(const std::string &gameName) {
        RpcMethod_P1(ListRoom, gameName);
    }

    JoinRoomRet JoinRoom(int roomId) {
        RpcMethod_P1(JoinRoom, roomId);
    }

    ExitRoomRet ExitRoom() {
        RpcMethod(ExitRoom);
    }

    QueryRoomRet QueryRoom(int roomId) {
        RpcMethod_P1(QueryRoom, roomId);
    }

    OperationInRoomRet OperationInRoom(Ready ready) {
        RpcMethod_P1(OperationInRoom, ready);
    }

    template<typename UserOperationT>
    OperationInRoomRet OperationInRoom(const UserOperationT &custom) {
        RpcMethod_P1(OperationInRoom, custom);
    }

    MessageRet Message(const std::string &message, const std::string &dstUser,
        int dstRoom) {
        RpcMethod_P3(Message, message, dstUser, dstRoom);
    }

#undef RpcMethodHelper
#undef RpcMethod
#undef RpcMethod_P1
#undef RpcMethod_P2
#undef RpcMethod_P3
#undef RpcMethod_P4

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

/// TODO: use a concrete one for each game including no-game
class ComponentWithUIProxy : public ftxui::Component, public UIProxyClient {

};

}}