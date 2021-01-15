#include "dock.h"

#include <sailgame_pb/core/types.pb.h>

namespace SailGame { namespace Dock {

using namespace ftxui;
using ::Core::ErrorNumber;

Dock::Dock(const std::shared_ptr<UIProxy> &uiProxy)
    : mUIProxy(uiProxy)
{
    Add(&mScreenContainer);
    mScreenContainer.Add(&mLoginScreen);
    mScreenContainer.Add(&mLobbyScreen);
    mScreenContainer.Add(&mRoomScreen);
    mScreenContainer.Add(&mGameScreen);

    mLoginScreen.SetUIProxy(mUIProxy.get());
    mLobbyScreen.SetUIProxy(mUIProxy.get());
    mRoomScreen.SetUIProxy(mUIProxy.get());
    mGameScreen.SetUIProxy(mUIProxy.get());

    // navigation between screens
    mLoginScreen.OnLogin = [this](const auto& ret) {
        assert(ret.err() == ErrorNumber::OK);
        spdlog::info("login success");
        auto username = ret.account().username();
        mUIProxy->OnLoginSuccess(ret.token(), username);
        mLobbyScreen.mUsername = username;
        mLobbyScreen.mPoints = ret.account().points();
        mLobbyScreen.TakeFocus();
    };

    mLobbyScreen.OnJoinRoom = [this] { mRoomScreen.TakeFocus(); };

    mRoomScreen.OnExitRoom = [this] { mLobbyScreen.TakeFocus(); };

    mRoomScreen.OnGameStart = [this] {
        auto gameName = dynamic_cast<const State &>(mUIProxy->GetState())
            .mRoomDetails.gamename();

        mUIProxy->SwitchToNewStateMachine(
            GameAttrFactory::Create(gameName)->GetStateMachine());

        mGameScreen.SwitchToNewGameScreen(
            GameAttrFactory::Create(gameName)->GetGameScreen());
        mGameScreen.TakeFocus();
    };

    mGameScreen.OnGameOver = [this] {
        // mUIProxy->SwitchToNewStateMachine(StateMachine::Create());
        mGameScreen.ResetGameScreen();
        mRoomScreen.TakeFocus();
    };
}

Dock::~Dock() {}

void Dock::Loop() {
    mScreen.Loop(this);
}

}}