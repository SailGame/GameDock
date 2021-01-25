#pragma once

#include <sailgame/common/state_machine.h>
#include <sailgame/common/game_manager.h>

namespace SailGame { namespace Common {

class ClientStateMachine : public IStateMachine {
public:
    void TransitionForBroadcastMsg(const BroadcastMsg &msg) {
        Transition(msg);
    }

    virtual void SetState(const IState &) = 0;

    virtual GameType GetType() const = 0;

    virtual void SwitchFrom(const ClientStateMachine &) = 0;

    virtual Dock::State SwitchToRoom() const = 0;

protected:
    virtual void Transition(const BroadcastMsg &) = 0;
};

class ClientNetworkInterface : public INetworkInterface {
public:
    ClientNetworkInterface(const std::shared_ptr<GameCore::StubInterface> &stub)
        : INetworkInterface(stub)
    {
        mListenFunc = [this] {
            while (true) {
                OnEventHappens(ReceiveMsg());
            }
        };
    }

    static std::shared_ptr<ClientNetworkInterface> Create(
        const std::shared_ptr<GameCore::StubInterface> &stub) 
    {
        return std::make_shared<ClientNetworkInterface>(stub);
    }

    virtual void Connect() override {
        spdlog::info("[INetworkInterface] Connect invoked.");
        mStream = mStub->Listen(&mContext, CoreMsgBuilder::CreateListenArgs());
    }

    virtual bool IsConnected() const override {
        return mStream != nullptr;
    }

    void OnEventHappens(const BroadcastMsg &msg) {
        mSubscriber->OnEventHappens(std::make_shared<BroadcastMsgEvent>(msg));
    }

    BroadcastMsg ReceiveMsg()
    {
        BroadcastMsg msg;
        if (mStream->Read(&msg)) {
            return msg;
        }
        std::cout << mStream->Finish().error_message() << std::endl;
        if (!mStream->Finish().ok()) {
            std::cout << "rpc failed." << std::endl;
            std::exit(-1);
        }
        auto error_msg = "Stream ends normally, which indicates error in core side.";
        throw std::runtime_error(error_msg);
        return msg;
    }

#define RpcMethod(RpcName) \
    Core::RpcName##Ret RpcName(const Core::RpcName##Args &args) { \
        RpcName##Ret ret; \
        ClientContext context; \
        auto status = mStub->RpcName(&context, args, &ret); \
        return ret; \
    }

    RpcMethod(Login)
    RpcMethod(QueryAccount)
    RpcMethod(CreateRoom)
    RpcMethod(ControlRoom)
    RpcMethod(ListRoom)
    RpcMethod(JoinRoom)
    RpcMethod(ExitRoom)
    RpcMethod(QueryRoom)
    RpcMethod(OperationInRoom)
    RpcMethod(Message)
#undef RpcMethod

private:
    std::shared_ptr<ClientReaderInterface<BroadcastMsg>> mStream;
};

class ClientGameManager : public IGameManager {
public:
    ClientGameManager(const std::shared_ptr<EventLoop> &eventLoop, 
        const std::shared_ptr<ClientStateMachine> &stateMachine,
        const std::shared_ptr<ClientNetworkInterface> &networkInterface)
        : IGameManager(eventLoop), mStateMachine(stateMachine),
        mNetworkInterface(networkInterface)
    {
        mNetworkInterface->SetSubscriber(this);
    }

    virtual void Start() override {
        mNetworkInterface->Connect();
        IGameManager::Start();
    }

    virtual void Stop() override { 
        mNetworkInterface->Stop();
        IGameManager::Stop();
    }

    virtual void OnEventProcessed(const EventPtr &event) override {
        assert(event->mType == EventType::BROADCAST_MSG);
        auto msg = std::dynamic_pointer_cast<BroadcastMsgEvent>(event)->mMsg;
        mStateMachine->TransitionForBroadcastMsg(msg);
    }

    void StartWithToken(const std::string &token) {
        CoreMsgBuilder::SetToken(token);
        mNetworkInterface->AsyncListen();
        IGameManager::Start();
    }
    
    void SwitchStateMachine(
        const std::shared_ptr<ClientStateMachine> &newStateMachine) 
    {
        auto oldStateMachine = mStateMachine;
        mStateMachine = newStateMachine;
        mStateMachine->SwitchFrom(*oldStateMachine);
    }

    GameType GetGameType() const { return mStateMachine->GetType(); }

    const IState &GetState() const { return mStateMachine->GetState(); }

    void SetState(const IState &state) { mStateMachine->SetState(state); }

    std::shared_ptr<ClientStateMachine> GetStateMachine() const {
        return mStateMachine;
    }

private:
    std::shared_ptr<ClientStateMachine> mStateMachine;
    std::shared_ptr<ClientNetworkInterface> mNetworkInterface;
};

}}