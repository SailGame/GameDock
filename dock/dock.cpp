#include "dock.h"

#include <sailgame_pb/core/types.pb.h>

namespace SailGame { namespace Dock {

using namespace ftxui;
using ::Core::ErrorNumber;

Dock::Dock(const std::shared_ptr<UIProxy> &uiProxy) : mUIProxy(uiProxy) {
    mScreenContainer.Add(&mLoginScreen);
    mScreenContainer.Add(&mLobbyScreen);
    mScreenContainer.Add(&mRoomScreen);
    mScreenContainer.Add(&mPolyGameScreen);

    mLoginScreen.SetUIProxy(mUIProxy.get());
    mLobbyScreen.SetUIProxy(mUIProxy.get());
    mRoomScreen.SetUIProxy(mUIProxy.get());
    mPolyGameScreen.SetUIProxy(mUIProxy.get());

    // mPolyGameScreen.SetComponent(std::make_shared<GameScreen>());

    // navigation between screens
    mLoginScreen.OnLogin = [this](const auto &ret) {
        assert(ret.err() == ErrorNumber::OK);
        spdlog::info("login success");
        auto username = ret.account().username();
        mUIProxy->OnLoginSuccess(ret.token(), username);
        mLobbyScreen.mUsername = username;
        mLobbyScreen.mPoints = ret.account().points();
        mLobbyScreen.TakeFocus();
    };

    mLobbyScreen.OnJoinRoom = [this](int roomId) {
        auto ret = mUIProxy->QueryRoom(roomId);
        assert(ret.err() == ErrorNumber::OK);
        auto state = dynamic_cast<const State &>(mUIProxy->GetState());
        state.mRoomDetails = ret.room();
        mUIProxy->SetState(state);
        mRoomScreen.TakeFocus();
    };

    mRoomScreen.OnExitRoom = [this] { mLobbyScreen.TakeFocus(); };

    mUIProxy->OnGameStart = [this](GameType game) {
        mUIProxy->SwitchToNewStateMachine(
            GameAttrFactory::Create(game)->GetStateMachine());
        mPolyGameScreen.SetComponent(
            GameAttrFactory::Create(game)->GetGameScreen());
        mPolyGameScreen.TakeFocus();

        mPolyGameScreen.Invoke(&GameScreen::RegisterGameOverCallback, [this] {
            spdlog::info("Game Over callback invoked");
            mUIProxy->SwitchToNewStateMachine(StateMachine::Create());
            mPolyGameScreen.ResetComponent();
            mRoomScreen.TakeFocus();
        });
    };
}

Dock::~Dock() {}

void Dock::Loop(bool useRefersher) {
    /// XXX: where to join
    std::unique_ptr<std::thread> refersher;
    if (useRefersher) {
        refersher = std::make_unique<std::thread>([&] {
            while (true) {
                using namespace std::chrono_literals;
                std::this_thread::sleep_for(0.05s);
                mScreen.PostEvent(Event::Custom);
            }
        });
    }
    mScreen.Loop(&mScreenContainer);
}

}}  // namespace SailGame::Dock