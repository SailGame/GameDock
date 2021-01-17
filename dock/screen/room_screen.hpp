#pragma once

#include <ftxui/component/container.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/button.hpp>
#include <sailgame_pb/core/types.pb.h>
#include <sailgame/common/core_msg_builder.h>

#include "../core/ui_proxy.h"
#include "../util/dom.hpp"
#include "../component/game_settings_ctrl.hpp"
#include "../component/poly_component.hpp"
#include "../component/empty_component.hpp"

namespace SailGame { namespace Dock {

using namespace ftxui;

using ::Core::Ready;
using ::Core::RoomDetails;
using ::Core::RoomUser;
using SailGame::Common::CoreMsgBuilder;
using SailGame::Dock::State;

class RoomScreen : public ComponentWithUIProxy {
public:
    std::function<void()> OnExitRoom;

    std::function<void()> OnGameStart;

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

        mReadyToggleButton.on_click = [this] {
            if (!mIsReady) {
                mUIProxy->OperationInRoom(
                    CoreMsgBuilder::CreateOperationInRoomArgs(Ready::READY));
            }
            else {
                mUIProxy->OperationInRoom(
                    CoreMsgBuilder::CreateOperationInRoomArgs(Ready::CANCEL));
            }
            mIsReady = !mIsReady;
        };

        mExitRoomButton.on_click = [this] {
            auto ret = mUIProxy->ExitRoom(CoreMsgBuilder::CreateExitRoomArgs());
            if (ret.err() == ErrorNumber::OK) {
                OnExitRoom();
            }
        };

        mSetButton.on_click = [this] {
            mSettingButtonsContainer.SetActiveChild(
                &mSaveOrCancelButtonsContainer);
            mGameSettingsController.Invoke(
                &GameSettingsController::ControlMode);
        };

        mSaveChangeButton.on_click = [this] {
            auto results = mGameSettingsController.Invoke(
                &GameSettingsController::GetControlResults);
            auto roomId = GetState().mRoomDetails.roomid();
            auto gameName = GetState().mRoomDetails.gamename();
            auto roomPassword = "";
            mUIProxy->ControlRoom(CoreMsgBuilder::CreateControlRoomArgs(
                roomId, gameName, roomPassword, results));
            mSetButton.TakeFocus();
            mGameSettingsController.Invoke(
                &GameSettingsController::ReadOnlyMode);
        };

        mCancelChangeButton.on_click = [this] {
            mSetButton.TakeFocus();
            mGameSettingsController.Invoke(
                &GameSettingsController::ReadOnlyMode);
        };
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
            !mGameSettingsController.HasComponent()) 
        {
            /// TODO: switch GameSettingsController if change game
            mGameSettingsController.SetComponent(
                GameAttrFactory::Create(GetState().mRoomDetails.gamename())
                    ->GetGameSettingsController());
        }

        // OnGameStart callback will switch state machine,
        // so it should be invoked after reading details from state machine
        if (AreAllUsersReady()) {
            OnGameStart();
        }
    }

    Element Render() final {
        // Remember that details could be empty
        // because state machine is updated asynchrously.
        auto details = GetState().mRoomDetails;
        Update();

        auto topBar = hbox({
            text(L"roomId: "),
            text(to_wstring(details.roomid())),
            filler(),
            text(L"game: "),
            text(to_wstring(details.gamename()))
        });

        auto playerList = vbox({
            text(L"Player List"),
            separator(),
            Dom::MapVectorToVBox(
                details.user(),
                &Dom::RoomUserToHBox
            )
        }) | flex | width(25);

        auto roomDetail = vbox({
            text(L"Room Detail"),
            separator(),
            text(L"Game settings") | bold,
            mGameSettingsContainer.Render()
        }) | xflex;

        auto doc = vbox({
            topBar,
            separator(),
            hbox({
                filler(),
                mReadyToggleButton.Render(),
                filler(),
                mExitRoomButton.Render(),
                filler()
            }),
            separator(),
            hbox({
                playerList,
                separator(),
                roomDetail                    
            }) | yflex
        });

        return doc | range(80, 25) | border | center;
    }

    State GetState() const {
        return dynamic_cast<const State &>(mUIProxy->GetState());
    }

    bool AreAllUsersReady() const {
        auto roomUsers = GetState().mRoomDetails.user();
        if (roomUsers.empty()) {
            return false;
        }
        for (const auto &roomUser : roomUsers) {
            if (roomUser.userstate() != RoomUser::READY) {
                return false;
            }
        }
        return true;
    }

public:
    bool mIsReady{false};

public:
// private:
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
}}