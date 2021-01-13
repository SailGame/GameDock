#pragma once

#include <ftxui/component/container.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/menu.hpp>
#include <ftxui/component/input.hpp>
#include <ftxui/component/button.hpp>
#include <ftxui/screen/string.hpp>

#include <sailgame_pb/core/types.pb.h>
#include <sailgame_pb/core/error.pb.h>
#include <sailgame/common/util.h>

#include "../component/non_border_button.hpp"
#include "../util/util.hpp"
#include "../core/ui_proxy.h"

namespace SailGame { namespace Dock {

using namespace ftxui;

using ::Core::ErrorNumber;
using ::Core::Room;
using ::Core::JoinRoomRet;
using ::Core::RoomDetails;
using SailGame::Common::CoreMsgBuilder;
using SailGame::Common::Util;
using SailGame::Dock::NonBorderButton;
using SailGame::Dock::DockUtil;

class LobbyScreen : public Component, public UIProxyClient {
public:
    std::function<void()> OnJoinRoom;

    LobbyScreen() {
        Add(&mContainer);
        mContainer.Add(&mSearchBar);
        mContainer.Add(&mRoomDisplayArea);

        mSearchBar.Add(&mSearchInput);
        mSearchBar.Add(&mSearchButton);

        mRoomDisplayArea.Add(&mRoomList);
        mRoomDisplayArea.Add(&mRoomDetail);

        mRoomDetail.Add(&mJoinRoomButton);

        mSearchInput.placeholder = L"input game name";
        mSearchButton.on_click = [this] {
            mRooms = Util::ConvertGrpcRepeatedPtrFieldToVector(
                mUIProxy->ListRoom(CoreMsgBuilder::CreateListRoomArgs(
                    to_string(mSearchInput.content)
                )).room()
            );
            if (!mRooms.empty()) {
                mRoomList.selected = 0;
                mDetails = mUIProxy->QueryRoom(
                    CoreMsgBuilder::CreateQueryRoomArgs(
                        mRooms[0].roomid())).room();
            }
        };

        mRoomList.on_change = [this] {
            mDetails = mUIProxy->QueryRoom(CoreMsgBuilder::CreateQueryRoomArgs(
                mRooms[mRoomList.selected].roomid())).room();
        };

        mJoinRoomButton.on_click = [this] {
            auto roomId = mRooms[mRoomList.selected].roomid();
            auto ret = mUIProxy->JoinRoom(CoreMsgBuilder::CreateJoinRoomArgs(
                roomId));
            if (ret.err() == ErrorNumber::OK) {
                OnJoinRoom();
            }
        };
    }

    void Update() {
        DockUtil::MapVectorToMenuEntries(mRoomList, mRooms, 
            &DockUtil::RoomToString);
    }

    Element Render() final {
        Update();

        auto doc = vbox({
            hbox({
                text(L"username: "),
                text(to_wstring(mUsername)),
                filler(),
                text(L"points: "),
                text(to_wstring(mPoints))
            }),
            separator(),
            mSearchBar.Render(),
            separator(),
            hbox({
                vbox({
                    text(L"Room List"),
                    separator(),
                    mRoomList.Render() 
                }) | size(HEIGHT, GREATER_THAN, 18)
                | size(WIDTH, GREATER_THAN, 20),
                separator(),
                (mDetails.gamename().empty() ?
                    text(L"choose a room to view details") :
                    vbox({
                        text(L"Room Detail"),
                        separator(),
                        text(to_wstring(mDetails.gamename())),
                        text(to_wstring(mDetails.roomid())),
                        separator(),
                        DockUtil::MapVectorToVBox(
                            DockUtil::GetUsernamesFromRoomDetails(mDetails),
                            &DockUtil::UserNameToText
                        ),
                        DockUtil::ShowGameSettings(mDetails),
                        mJoinRoomButton.Render()
                })) | size(WIDTH, GREATER_THAN, 25)
            })
        }) | border;

        return doc | 
            center;
    }

/// XXX: aggregate this states as one
public:
    std::string mUsername;
    int mPoints{0};
    std::vector<Room> mRooms;
    RoomDetails mDetails;

public:
// private:
    Container mContainer{Container::Vertical()};
    Container mSearchBar{Container::Horizontal()};
    Input mSearchInput;
    NonBorderButton mSearchButton{L"Search"};
    Container mRoomDisplayArea{Container::Horizontal()};
    Menu mRoomList;
    Container mRoomDetail{Container::Vertical()};
    Button mJoinRoomButton{L"Join"};
};

}}