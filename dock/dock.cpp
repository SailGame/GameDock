#include "dock.h"

#include <sailgame_pb/core/types.pb.h>

namespace SailGame { namespace Dock {

using namespace ftxui;
using ::Core::ErrorNumber;

Dock::Dock(const std::shared_ptr<UIProxy> &uiProxy) : mUIProxy(uiProxy) {
    mLoginScreen = std::make_shared<LoginScreen>();
    mLobbyScreen = std::make_shared<LobbyScreen>();
    mRoomScreen = std::make_shared<RoomScreen>();
    mPolyGameScreen = std::make_shared<PolyComponent<GameScreen>>();

    mLoginScreen->SetUIProxy(mUIProxy.get());
    mLobbyScreen->SetUIProxy(mUIProxy.get());
    mRoomScreen->SetUIProxy(mUIProxy.get());
    mPolyGameScreen->SetUIProxy(mUIProxy.get());

    // navigation between screens
    mLoginScreen->OnLogin = [this](const auto &ret) {
        assert(ret.err() == ErrorNumber::OK);
        spdlog::info("login success");
        auto username = ret.account().username();
        mUIProxy->OnLoginSuccess(ret.token(), username);
        mLobbyScreen->mUsername = username;
        mLobbyScreen->mPoints = ret.account().points();
        mLobbyScreen->TakeFocus();
    };

    mLobbyScreen->OnJoinRoom = [this](int roomId) {
        QueryRoomAndSetStateMachine(roomId);
    };

    mRoomScreen->OnExitRoom = [this] { mLobbyScreen->TakeFocus(); };

    mUIProxy->OnGameStart = [this](GameType game) {
        mUIProxy->SwitchToNewStateMachine(
            GameAttrFactory::Create(game)->GetStateMachine());
        mPolyGameScreen->SetComponent(
            GameAttrFactory::Create(game)->GetGameScreen());
        mPolyGameScreen->TakeFocus();

        mPolyGameScreen->Invoke(&GameScreen::RegisterGameOverCallback, [this] {
            spdlog::info("Game Over callback invoked");
            mUIProxy->SwitchToNewStateMachine(StateMachine::Create());
            mUIProxy->SetGameStartCallback();
            mPolyGameScreen->ResetComponent();
            auto roomId = dynamic_cast<const State &>(mUIProxy->GetState())
                              .mRoomDetails.roomid();
            QueryRoomAndSetStateMachine(roomId);
        });
    };

    mUIProxy->OnExitApp = [this] {
        /// TODO: inform Core to logout
        mScreen.ExitLoopClosure()();
        mUIProxy->Stop();
    };

    mScreenContainer = Container::Tab(
        {mLoginScreen, mLobbyScreen, mRoomScreen, mPolyGameScreen},
        &mScreenContainerSelected);
}

void Dock::QueryRoomAndSetStateMachine(int roomId) {
    auto ret = mUIProxy->QueryRoom(roomId);
    assert(ret.err() == ErrorNumber::OK);
    auto state = dynamic_cast<const State &>(mUIProxy->GetState());
    state.mRoomDetails = ret.room();
    mUIProxy->SetState(state);
    mRoomScreen->TakeFocus();
}

void Dock::Loop(bool useRefresher) {
    std::unique_ptr<std::thread> refresher;
    bool shouldStop = false;
    if (useRefresher) {
        refresher = std::make_unique<std::thread>([&] {
            while (!shouldStop) {
                using namespace std::chrono_literals;
                std::this_thread::sleep_for(0.05s);
                mScreen.PostEvent(Event::Custom);
            }
        });
    }
    mScreen.Loop(mScreenContainer);
    shouldStop = true;
    if (refresher) {
        refresher->join();
    }
    std::cout << "Goodbye, Sailor." << std::endl;
}

}}  // namespace SailGame::Dock