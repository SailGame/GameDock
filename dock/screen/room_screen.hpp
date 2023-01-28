#pragma once

#include <sailgame/common/core_msg_builder.h>
#include <sailgame_pb/core/types.pb.h>

#include <ftxui/component/component.hpp>

#include "../component/empty_component.hpp"
#include "../component/game_settings_ctrl.hpp"
#include "../component/poly_component.hpp"
#include "../core/component.h"
#include "../core/game_attr_fac.h"
#include "../util/dom.hpp"

namespace SailGame { namespace Dock {

using namespace ftxui;

using ::Core::Ready;
using ::Core::RoomDetails;
using ::Core::RoomUser;
using SailGame::Common::CoreMsgBuilder;
using SailGame::Dock::State;

static const std::vector<std::string> SupportedGames = {"No", "UNO", "Texas"};

class RoomScreen : public DockComponent {
public:
    std::function<void()> OnExitRoom;

    RoomScreen() {
        for (const auto& gameName : SupportedGames) {
            try {
                mGameAttrs.emplace(gameName, GameAttrFactory::Create(gameName));
            } catch (const std::exception& e) {
            }
        }

        mGameSettingsController =
            std::make_shared<PolyComponent<GameSettingsController>>();
        mEmptyComponent = std::make_shared<EmptyComponent>();

        auto gameListOption = RadioboxOption::Simple();
        gameListOption.on_change = [this] {
            if (mGameSelected != 0) {
                mGameSettingsController->SetComponent(
                    mGameAttrs.at(GetGameName())->GetGameSettingsController());
                ControlRoom();
            } else {
                mGameSettingsController->ResetComponent();
            }
        };
        mGameList = Radiobox(&SupportedGames, &mGameSelected,
                             std::move(gameListOption));

        mReadyToggleButtonLabel = L"Ready";
        mReadyToggleButton =
            Button(&mReadyToggleButtonLabel, [this] { ToggleReady(); });
        mExitRoomButton = Button(L"Exit Room", [this] { ExitRoom(); });
        mSetButton = Button(L"Change settings", [this] { EnterControlMode(); });
        mSaveChangeButton = Button(L"Save", [this] { ControlRoom(); });
        mCancelChangeButton = Button(L"Cancel", [this] { QuitControlMode(); });

        mButtonsContainer = Container::Horizontal(
            {mGameList, mReadyToggleButton, mExitRoomButton});

        mSaveOrCancelButtonsContainer =
            Container::Horizontal({mSaveChangeButton, mCancelChangeButton});

        mSettingButtonsContainer = Container::Tab(
            {mEmptyComponent, mSetButton, mSaveOrCancelButtonsContainer},
            &mSettingButtonsContainerSelected);

        mGameSettingsContainer = Container::Vertical(
            {mGameSettingsController, mSettingButtonsContainer});

        mContainer->Add(mButtonsContainer);
        mContainer->Add(mGameSettingsContainer);
    }

    virtual void SetUIProxy(UIProxy* uiProxy) override {
        UIProxyClient::SetUIProxy(uiProxy);
        mGameSettingsController->SetUIProxy(uiProxy);
    }

    void Update() {
        mReadyToggleButtonLabel = mIsReady ? L"Cancel" : L"Ready";
        if (GetState().IsOwner() && mEmptyComponent->Active()) {
            // cannot write like mSetButton.TakeFocus()
            // because when the user is granted ownership,
            // his focus could be elsewhere.
            mSettingButtonsContainer->SetActiveChild(mSetButton.get());
        }

        if (GetState().IsOwner() && mGameSettingsContainer->Focused()) {
            mContainer->SetActiveChild(mButtonsContainer.get());
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
                                mGameSettingsContainer->Render()}) |
                          xflex;

        auto doc = vbox({topBar, separator(),
                         hbox({filler(), mGameList->Render(), filler(),
                               mReadyToggleButton->Render(), filler(),
                               mExitRoomButton->Render(), filler()}),
                         separator(),
                         hbox({playerList, separator(), roomDetail}) | yflex});

        return TryRenderDialog(doc) | range(80, 25) | border | center;
    }

    virtual void TakeFocus() override {
        mIsReady = false;
        SailGameComponent::TakeFocus();
    }

private:
    void ToggleReady() {
        OperationInRoomRet ret;
        if (!mIsReady) {
            ret = mUIProxy->OperationInRoom(Ready::READY);
        } else {
            ret = mUIProxy->OperationInRoom(Ready::CANCEL);
        }
        if (ret.err() != ErrorNumber::OK) {
            ShowDialogWithText(ErrorNumber_Name(ret.err()));
            return;
        }
        mIsReady = !mIsReady;
    }

    void ExitRoom() {
        auto ret = mUIProxy->ExitRoom();
        if (ret.err() != ErrorNumber::OK) {
            ShowDialogWithText(ErrorNumber_Name(ret.err()));
            return;
        }
        OnExitRoom();
    }

    void EnterControlMode() {
        mSettingButtonsContainer->SetActiveChild(
            mSaveOrCancelButtonsContainer.get());
        mGameSettingsController->Invoke(&GameSettingsController::ControlMode);
    }

    void ControlRoom() {
        auto results = mGameSettingsController->Invoke(
            &GameSettingsController::GetControlResults);
        auto roomId = GetState().mRoomDetails.roomid();
        auto gameName = GetGameName();
        auto roomPassword = "";
        auto ret =
            mUIProxy->ControlRoom(roomId, gameName, roomPassword, results);
        if (ret.err() != ErrorNumber::OK) {
            ShowDialogWithText(ErrorNumber_Name(ret.err()));
            mGameSettingsController->ResetComponent();
            mGameSelected = 0;
            return;
        }
        mSetButton->TakeFocus();
        mGameSettingsController->Invoke(&GameSettingsController::ReadOnlyMode);
    }

    const std::string& GetGameName() {
        return SupportedGames.at(mGameSelected);
    }

    void QuitControlMode() {
        mSetButton->TakeFocus();
        mGameSettingsController->Invoke(&GameSettingsController::ReadOnlyMode);
    }

public:
    bool mIsReady{false};
    int mSettingButtonsContainerSelected{0};
    std::wstring mReadyToggleButtonLabel;
    std::unordered_map<std::string, std::unique_ptr<IGameAttr>> mGameAttrs;
    int mGameSelected{0};

public:
    Component mGameList;
    Component mButtonsContainer;
    Component mReadyToggleButton;
    Component mExitRoomButton;
    Component mGameSettingsContainer;
    std::shared_ptr<PolyComponent<GameSettingsController>>
        mGameSettingsController;
    Component mSettingButtonsContainer;
    std::shared_ptr<EmptyComponent> mEmptyComponent;
    Component mSetButton;
    Component mSaveOrCancelButtonsContainer;
    Component mSaveChangeButton;
    Component mCancelChangeButton;
};

}}  // namespace SailGame::Dock