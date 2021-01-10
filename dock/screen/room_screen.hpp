#pragma once

#include <ftxui/component/container.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/button.hpp>
#include <sailgame_pb/core/types.pb.h>
#include <sailgame/common/core_msg_builder.h>

#include "../core/ui_proxy.h"
#include "../core/state.h"

namespace SailGame { namespace Dock {

using namespace ftxui;

using ::Core::Ready;
using ::Core::RoomDetails;
using ::Core::RoomUser;
using SailGame::Common::CoreMsgBuilder;
using SailGame::Dock::DockUtil;
using SailGame::Dock::State;

class RoomScreen : public Component {
public:
    std::function<void()> OnExitRoom;

    std::function<void()> OnGameStart;

    RoomScreen() {
        Add(&mContainer);
        mContainer.Add(&mReadyToggleButton);
        mContainer.Add(&mExitRoomButton);

        mReadyToggleButton.on_click = [this] {
            if (mIsReady) {
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
    }

    void SetUIProxy(UIProxy *uiProxy) { mUIProxy = uiProxy; }

    void Update() {
        mReadyToggleButton.label = mIsReady ? L"Cancel" : L"Ready";
        if (AreAllUsersReady()) {
            OnGameStart();
        }
    }

    Element Render() final {
        Update();
        auto details = GetState().mRoomDetails;

        auto doc = hbox({
            vbox({
                hbox({
                    mReadyToggleButton.Render(),
                    filler(),
                    mExitRoomButton.Render()
                }),
                separator(),
                // playerlist
                DockUtil::MapVectorToVBox(
                    details.user(),
                    &DockUtil::RoomUserToText
                )
            }),
            separator(),
            vbox({
                text(L"Room Detail"),
                separator(),
                /// TODO: owner can change game and its settings
                text(to_wstring(details.gamename())),
                text(to_wstring(details.roomid())),
                separator(),
                DockUtil::ShowGameSettings(details),
            })
        }) | border;

        return doc | center;
    }

    State GetState() const {
        return dynamic_cast<const State &>(mUIProxy->GetState());
    }

    bool AreAllUsersReady() const {
        for (const auto &roomUser : GetState().mRoomDetails.user()) {
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
    UIProxy *mUIProxy;
    Container mContainer{Container::Horizontal()};
    Button mReadyToggleButton{L"Ready"};
    Button mExitRoomButton{L"Exit Room"};
};

}}