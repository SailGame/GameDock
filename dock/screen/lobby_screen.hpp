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

#include "../core/ui_proxy.h"
#include "../component/non_border_button.hpp"
#include "../component/text_center_button.hpp"
#include "../util/util.hpp"
#include "../util/dom.hpp"

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

class LobbyScreen : public ComponentWithUIProxy {
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
        Dom::MapVectorToMenuEntries(mRoomList, mRooms, 
            &Dom::RoomToString);
    }

    Element Render() final {
        Update();

        auto topBar = hbox({
            text(L"username: "),
            text(to_wstring(mUsername)),
            filler(),
            text(L"points: "),
            text(to_wstring(mPoints))
        });

        auto roomList = vbox({
            text(L"Room List"),
            separator(),
            mRoomList.Render() 
        }) | flex | width(20);

        auto roomDetail = vbox({
            text(L"Room Detail"),
            separator(),
            hbox({
                text(L"Game: ") | bold,
                text(to_wstring(mDetails.gamename())) | flex,
                text(L"RoomId: ") | bold,
                text(to_wstring(mDetails.roomid())) | flex,
            }),
            separator(),
            text(L"Players in the room") | bold,
            Dom::MapVectorToVBox(
                DockUtil::GetUsernamesFromRoomDetails(mDetails),
                &Dom::UserNameToText
            ),
            separator(),
            text(L"Game settings") | bold,
            Dom::ShowGameSettings(mDetails),
            separator(),
            mJoinRoomButton.Render()
        }) | xflex;

        auto doc = vbox({
            topBar,
            separator(),
            mSearchBar.Render(),
            separator(),
            hbox({
                roomList,
                separator(),
                (mDetails.gamename().empty() ?
                    text(L"choose a room to view details") :
                    roomDetail)
            }) | yflex
        });

        return doc | range(80, 25) | border | center;
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
    TextCenterButton mJoinRoomButton{L"Join"};
};

}}