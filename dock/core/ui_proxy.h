#pragma once

#include <memory>
#include <sailgame/common/game_manager.h>
#include <sailgame/common/network_interface.h>
#include <sailgame/common/core_msg_builder.h>
#include <sailgame_pb/core/types.pb.h>
#include <sailgame_pb/core/error.pb.h>

#include <sailgame/uno/msg_builder.h>

#include "state_machine.h"

namespace SailGame { namespace Dock {

using Common::CoreMsgBuilder;
using Common::EventLoop;
using Common::GameManager;
using Common::NetworkInterface;
using Common::IState;
using Common::IStateMachine;
using Common::NetworkInterface;
using namespace ::Core;

class UIProxy {
public:
    UIProxy(const std::shared_ptr<NetworkInterface<false>> &networkInterface,
        bool isTest = false)
        : mNetworkInterface(networkInterface), mIsTest(isTest) {}
    
    static std::shared_ptr<UIProxy> Create(
        const std::shared_ptr<NetworkInterface<false>> &networkInterface,
        bool isTest = false) 
    {
        return std::make_shared<UIProxy>(networkInterface, isTest);
    }

    void SwitchToNewStateMachine(
        const std::shared_ptr<IStateMachine> &stateMachine)
    {
        assert(mGameManager);
        mGameManager->SetNewStateMachine(stateMachine);
    }

    void OnLoginSuccess(const std::string &token, 
        const std::string &username = "test") 
    {
        assert(!mGameManager);
        mGameManager = std::make_shared<GameManager<false>>(
            EventLoop::Create(), 
            SailGame::Dock::StateMachine::Create(username), mNetworkInterface);
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
    }

    void Stop() {
        if (mGameManager) {
            assert(mGameManagerThread);
            mGameManager->Stop();
            mGameManagerThread->join();
        }
    }

    LoginRet Login(const LoginArgs &args) {
        return mNetworkInterface->Login(args);
    }

    QueryAccountRet QueryAccount(const QueryAccountArgs &args) {
        return mNetworkInterface->QueryAccount(args);
    }

    CreateRoomRet CreateRoom(const CreateRoomArgs &args) {
        return mNetworkInterface->CreateRoom(args);
    }

    ControlRoomRet ControlRoom(const ControlRoomArgs &args) {
        return mNetworkInterface->ControlRoom(args);
    }

    ListRoomRet ListRoom(const ListRoomArgs &args) {
        return mNetworkInterface->ListRoom(args);
    }

    JoinRoomRet JoinRoom(const JoinRoomArgs &args) {
        return mNetworkInterface->JoinRoom(args);
    }

    ExitRoomRet ExitRoom(const ExitRoomArgs &args) {
        return mNetworkInterface->ExitRoom(args);
    }

    QueryRoomRet QueryRoom(const QueryRoomArgs &args) {
        return mNetworkInterface->QueryRoom(args);
    }

    OperationInRoomRet OperationInRoom(
        const OperationInRoomArgs &args) {
            return mNetworkInterface->OperationInRoom(args);
        }

    MessageRet Message(const MessageArgs &args) {
        return mNetworkInterface->Message(args);
    }

    const IState &GetState() const {
        return mGameManager->GetState();
    };

// private:
public:
    std::shared_ptr<NetworkInterface<false>> mNetworkInterface;
    std::shared_ptr<GameManager<false>> mGameManager;
    std::unique_ptr<std::thread> mGameManagerThread;
    bool mIsTest{false};
};

class UIProxyClient {
public:
    virtual void SetUIProxy(UIProxy *uiProxy) { mUIProxy = uiProxy; }

protected:
    UIProxy *mUIProxy;
};

}}