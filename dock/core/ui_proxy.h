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
using Common::IState;
using Common::IStateMachine;
using Common::NetworkInterface;
// using ::Core::Account;
// using ::Core::BroadcastMsg;
// using ::Core::ControlRoomArgs;
// using ::Core::ControlRoomRet;
// using ::Core::CreateRoomArgs;
// using ::Core::CreateRoomRet;
// using ::Core::ErrorNumber;
// using ::Core::ExitRoomArgs;
// using ::Core::ExitRoomRet;
// using ::Core::JoinRoomArgs;
// using ::Core::JoinRoomRet;
// using ::Core::ListenArgs;
// using ::Core::ListenRet;
// using ::Core::ListRoomArgs;
// using ::Core::ListRoomRet;
// using ::Core::LoginArgs;
// using ::Core::LoginRet;
// using ::Core::MessageArgs;
// using ::Core::MessageRet;
// using ::Core::OperationInRoomArgs;
// using ::Core::OperationInRoomRet;
// using ::Core::QueryAccountArgs;
// using ::Core::QueryAccountRet;
// using ::Core::QueryRoomArgs;
// using ::Core::QueryRoomRet;
// using ::Core::Ready;
// using ::Core::Room;
// using ::Core::RoomDetails;
// using ::Core::RoomUser;
using namespace ::Core;

class IUIProxy {
public:
    virtual void SwitchToNewStateMachine(
        const std::shared_ptr<IStateMachine> &stateMachine) = 0;

    virtual void OnLoginSuccess(const std::string &token) = 0;

    virtual LoginRet Login(const LoginArgs &args) = 0;

    virtual QueryAccountRet QueryAccount(const QueryAccountArgs &args) = 0;

    virtual CreateRoomRet CreateRoom(const CreateRoomArgs &args) = 0;

    virtual ControlRoomRet ControlRoom(const ControlRoomArgs &args) = 0;

    virtual ListRoomRet ListRoom(const ListRoomArgs &args) = 0;

    virtual JoinRoomRet JoinRoom(const JoinRoomArgs &args) = 0;

    virtual ExitRoomRet ExitRoom(const ExitRoomArgs &args) = 0;

    virtual QueryRoomRet QueryRoom(const QueryRoomArgs &args) = 0;

    virtual OperationInRoomRet OperationInRoom(
        const OperationInRoomArgs &args) = 0;

    virtual MessageRet Message(const MessageArgs &args) = 0;

    virtual const IState &GetState() const = 0;
};

class UIProxy : public IUIProxy {
public:
    UIProxy(const std::shared_ptr<NetworkInterface<false>> &networkInterface)
        : mNetworkInterface(networkInterface) {}

    virtual void SwitchToNewStateMachine(
        const std::shared_ptr<IStateMachine> &stateMachine) override
    {
        assert(mGameManager);
        mGameManager->SetNewStateMachine(stateMachine);
    }

    virtual void OnLoginSuccess(const std::string &token) override {
        assert(!mGameManager);
        mGameManager = std::make_shared<GameManager<false>>(
            EventLoop::Create(), 
            SailGame::Dock::StateMachine::Create(), mNetworkInterface);
        /// XXX: where to join
        mGameManagerThread = std::make_unique<std::thread>(
            [&, this] {
                mGameManager->StartWithToken(token);
            }
        );
    }

    virtual LoginRet Login(const LoginArgs &args) override {
        return mNetworkInterface->Login(args);
    }

    virtual QueryAccountRet QueryAccount(const QueryAccountArgs &args) = 0;

    virtual CreateRoomRet CreateRoom(const CreateRoomArgs &args) = 0;

    virtual ControlRoomRet ControlRoom(const ControlRoomArgs &args) = 0;

    virtual ListRoomRet ListRoom(const ListRoomArgs &args) = 0;

    virtual JoinRoomRet JoinRoom(const JoinRoomArgs &args) = 0;

    virtual ExitRoomRet ExitRoom(const ExitRoomArgs &args) = 0;

    virtual QueryRoomRet QueryRoom(const QueryRoomArgs &args) = 0;

    virtual OperationInRoomRet OperationInRoom(
        const OperationInRoomArgs &args) = 0;

    virtual MessageRet Message(const MessageArgs &args) = 0;

    virtual const IState &GetState() const override {
        return mGameManager->GetState();
    };

private:
    std::shared_ptr<NetworkInterface<false>> mNetworkInterface;
    std::shared_ptr<GameManager<false>> mGameManager;
    std::unique_ptr<std::thread> mGameManagerThread;
};

class MockUIProxy : public UIProxy {
public:
    MockUIProxy(const std::shared_ptr<NetworkInterface<false>> &networkInterface)
        : UIProxy(networkInterface) {}

    virtual void SwitchToNewStateMachine(
        const std::shared_ptr<IStateMachine> &stateMachine) override {}

    virtual void OnLoginSuccess(const std::string &token) override {}

    virtual LoginRet Login(const LoginArgs &args) override {
        return CoreMsgBuilder::CreateLoginRet(ErrorNumber::OK, "", 
            CoreMsgBuilder::CreateAccount("tbc", 0));
    }

    virtual QueryAccountRet QueryAccount(const QueryAccountArgs &args) override {
        return CoreMsgBuilder::CreateQueryAccountRet(ErrorNumber::OK, 
            CoreMsgBuilder::CreateAccount("tbc", 12));
    }

    virtual CreateRoomRet CreateRoom(const CreateRoomArgs &args) override {
        return CoreMsgBuilder::CreateCreateRoomRet(ErrorNumber::OK, 428);
    }

    virtual ControlRoomRet ControlRoom(const ControlRoomArgs &args) override {
        return CoreMsgBuilder::CreateControlRoomRet(ErrorNumber::OK, 
            Uno::MsgBuilder::CreateStartGameSettings(
                true, true, false, false, 15
            ));
    }

    virtual ListRoomRet ListRoom(const ListRoomArgs &args) override {
        return CoreMsgBuilder::CreateListRoomRet(ErrorNumber::OK, {
            CoreMsgBuilder::CreateRoom("UNO", 101, {"tbc", "tbc2"}),
            CoreMsgBuilder::CreateRoom("UNO", 102, {"tbc3"})
        });
    }

    virtual JoinRoomRet JoinRoom(const JoinRoomArgs &args) override {
        return CoreMsgBuilder::CreateJoinRoomRet(ErrorNumber::OK);
    }

    virtual ExitRoomRet ExitRoom(const ExitRoomArgs &args) override {
        return CoreMsgBuilder::CreateExitRoomRet(ErrorNumber::OK);
    }

    virtual QueryRoomRet QueryRoom(const QueryRoomArgs &args) override {
        return CoreMsgBuilder::CreateQueryRoomRet(ErrorNumber::OK,
            CoreMsgBuilder::CreateRoomDetails("UNO", 102, {
                CoreMsgBuilder::CreateRoomUser("tbc1", RoomUser::READY),
                CoreMsgBuilder::CreateRoomUser("tbc2", RoomUser::READY)
            }, Uno::MsgBuilder::CreateStartGameSettings(
                true, true, false, false, 15
            )));
    }

    virtual OperationInRoomRet OperationInRoom(
        const OperationInRoomArgs &args) override {
        return CoreMsgBuilder::CreateOperationInRoomRet(ErrorNumber::OK);
    }

    virtual MessageRet Message(const MessageArgs &args) override {
        return CoreMsgBuilder::CreateMessageRet(ErrorNumber::OK);
    }
};

}}