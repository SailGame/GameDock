#include "dock.h"

#include <thread>
#include <ftxui/component/menu.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/screen/string.hpp>
#include <sailgame/common/network_interface.h>
#include <sailgame/common/core_msg_builder.h>
#include <sailgame_pb/core/types.pb.h>

#include "../games/uno/src/state_machine.h"

namespace SailGame { namespace Dock {

using namespace ftxui;
using ::Core::ErrorNumber;
using SailGame::Common::CoreMsgBuilder;
using SailGame::Common::NetworkInterface;

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
        mUIProxy->OnLoginSuccess(ret.token());
        mLobbyScreen.mUsername = ret.account().username();
        mLobbyScreen.mPoints = ret.account().points();
        mLobbyScreen.TakeFocus();
    };

    mLobbyScreen.OnJoinRoom = [this] { mRoomScreen.TakeFocus(); };

    mRoomScreen.OnExitRoom = [this] { mLobbyScreen.TakeFocus(); };

    mRoomScreen.OnGameStart = [this] {
        auto gameName = dynamic_cast<const State &>(mUIProxy->GetState())
            .mRoomDetails.gamename();
        
        mUIProxy->SwitchToNewStateMachine(
            DockUtil::GetStateMachineByGameName(gameName));

        mGameScreen.SwitchToNewGameScreen(
            DockUtil::GetGameScreenByGameName(gameName));
        mGameScreen.TakeFocus();
    };

    mGameScreen.OnGameOver = [this] {
        mUIProxy->SwitchToNewStateMachine(StateMachine::Create());
        mGameScreen.ResetGameScreen();
        mRoomScreen.TakeFocus();
    };
}

Dock::~Dock() {}

void Dock::Loop() {
    mScreen.Loop(this);
}

}}