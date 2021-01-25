#pragma once

#include <sailgame/common/core_msg_builder.h>
#include <sailgame_pb/core/types.pb.h>

#include <ftxui/component/button.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/container.hpp>

#include "../component/empty_component.hpp"
#include "../component/game_settings_ctrl.hpp"
#include "../component/poly_component.hpp"
#include "../core/component.h"
#include "../util/dom.hpp"

namespace SailGame { namespace Dock {

using namespace ftxui;

using ::Core::Ready;
using ::Core::RoomDetails;
using ::Core::RoomUser;
using SailGame::Common::CoreMsgBuilder;
using SailGame::Dock::State;

class RoomScreen : public DockComponent {
public:
    std::function<void()> OnExitRoom;

    RoomScreen() {
        Add(&mContainer);
        mContainer.Add(&mButtonsContainer);
        mContainer.Add(&mGameSettingsContainer);

        mButtonsContainer.Add(&mReadyToggleButton);
        mButtonsContainer.Add(&mExitRoomButton);

        mGameSettingsContainer.Add(&mGameSettingsController);
        mGameSettingsContainer.Add(&mSettingButtonsContainer);

        mSettingButtonsContainer.Add(&mEmptyComponent);
        mSettingButtonsContainer.Add(&mSetButton);
        mSettingButtonsContainer.Add(&mSaveOrCancelButtonsContainer);

        mSaveOrCancelButtonsContainer.Add(&mSaveChangeButton);
        mSaveOrCancelButtonsContainer.Add(&mCancelChangeButton);

        mReadyToggleButton.on_click = [this] { ToggleReady(); };

        mExitRoomButton.on_click = [this] { ExitRoom(); };

        mSetButton.on_click = [this] { EnterControlMode(); };

        mSaveChangeButton.on_click = [this] { ControlRoom(); };

        mCancelChangeButton.on_click = [this] { QuitControlMode(); };
    }

    virtual void SetUIProxy(UIProxy *uiProxy) override {
        UIProxyClient::SetUIProxy(uiProxy);
        mGameSettingsController.SetUIProxy(uiProxy);
    }

    void Update() {
        mReadyToggleButton.label = mIsReady ? L"Cancel" : L"Ready";
        if (GetState().IsOwner()) {
            // if (true) {
            if (mEmptyComponent.Active()) {
                // cannot write like mSetButton.TakeFocus()
                // because when the user is granted ownership,
                // his focus could be elsewhere.
                mSettingButtonsContainer.SetActiveChild(&mSetButton);
            }
        }

        if (!GetState().mRoomDetails.gamename().empty() &&
            !mGameSettingsController.HasComponent()) {
            /// TODO: switch GameSettingsController if change game
            mGameSettingsController.SetComponent(
                GameAttrFactory::Create(GetState().mRoomDetails.gamename())
                    ->GetGameSettingsController());
        }
    }

    Element Render() final {
        // Remember that details could be empty
        // because state machine is updated asynchrously.
        RoomDetails details;
        try {
            details = GetState().mRoomDetails;
            Update();
        } catch (...) {
            return hbox();
        }

        auto topBar = hbox(
            {text(L"roomId: "), text(to_wstring(details.roomid())), filler(),
             text(L"game: "), text(to_wstring(details.gamename()))});

        auto playerList =
            vbox({text(L"Player List"), separator(),
                  Dom::MapVectorToVBox(details.user(), &Dom::RoomUserToHBox)}) |
            flex | width(25);

        auto roomDetail = vbox({text(L"Room Detail"), separator(),
                                text(L"Game settings") | bold,
                                mGameSettingsContainer.Render()}) |
                          xflex;

        auto doc = vbox({topBar, separator(),
                         hbox({filler(), mReadyToggleButton.Render(), filler(),
                               mExitRoomButton.Render(), filler()}),
                         separator(),
                         hbox({playerList, separator(), roomDetail}) | yflex});

        return doc | range(80, 25) | border | center;
    }

    virtual void TakeFocus() override {
        mIsReady = false;
        SailGameComponent::TakeFocus();
    }

private:
    void ToggleReady() {
        if (!mIsReady) {
            mUIProxy->OperationInRoom(Ready::READY);
        } else {
            mUIProxy->OperationInRoom(Ready::CANCEL);
        }
        mIsReady = !mIsReady;
    }

    void ExitRoom() {
        auto ret = mUIProxy->ExitRoom();
        assert(ret.err() == ErrorNumber::OK);
        OnExitRoom();
    }

    void EnterControlMode() {
        mSettingButtonsContainer.SetActiveChild(&mSaveOrCancelButtonsContainer);
        mGameSettingsController.Invoke(&GameSettingsController::ControlMode);
    }

    void ControlRoom() {
        auto results = mGameSettingsController.Invoke(
            &GameSettingsController::GetControlResults);
        auto roomId = GetState().mRoomDetails.roomid();
        auto gameName = GetState().mRoomDetails.gamename();
        auto roomPassword = "";
        mUIProxy->ControlRoom(roomId, gameName, roomPassword, results);
        mSetButton.TakeFocus();
        mGameSettingsController.Invoke(&GameSettingsController::ReadOnlyMode);
    }

    void QuitControlMode() {
        mSetButton.TakeFocus();
        mGameSettingsController.Invoke(&GameSettingsController::ReadOnlyMode);
    }

public:
    bool mIsReady{false};

public:
    Container mContainer{Container::Vertical()};
    Container mButtonsContainer{Container::Horizontal()};
    Button mReadyToggleButton{L"Ready"};
    Button mExitRoomButton{L"Exit Room"};
    Container mGameSettingsContainer{Container::Vertical()};
    PolyComponent<GameSettingsController> mGameSettingsController;
    Container mSettingButtonsContainer{Container::Tab(nullptr)};
    EmptyComponent mEmptyComponent;
    Button mSetButton{L"Change settings"};
    Container mSaveOrCancelButtonsContainer{Container::Horizontal()};
    Button mSaveChangeButton{L"Save"};
    Button mCancelChangeButton{L"Cancel"};
};
}}  // namespace SailGame::Dock