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
    std::function<void(int)> OnJoinRoom;

    LobbyScreen() {
        Add(&mContainer);
        mContainer.Add(&mOpBar);
        mContainer.Add(&mRoomDisplayArea);

        mOpBar.Add(&mSearchInput);
        mOpBar.Add(&mSearchButton);
        mOpBar.Add(&mCreateButton);
        mOpBar.Add(&mQueryAccountInput);
        mOpBar.Add(&mQueryAccountButton);

        mRoomDisplayArea.Add(&mRoomList);
        mRoomDisplayArea.Add(&mRoomDetail);

        mRoomDetail.Add(&mJoinRoomButton);

        mSearchInput.placeholder = L"game name or room id";
        mSearchButton.on_click = [this] { SearchRoom(); };

        mCreateButton.on_click = [this] { CreateRoom(); };

        mQueryAccountInput.placeholder = L"username";
        mQueryAccountButton.on_click = [this] { QueryAccount(); };

        mRoomList.on_change = [this] { QueryRoom(mRoomList.selected); };

        mJoinRoomButton.on_click = [this] { JoinRoom(); };
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

        auto roomElement = hbox({
            roomList,
            separator(),
            (mDetails.gamename().empty() ?
                text(L"choose a room to view details") :
                roomDetail)
        }) | yflex;

        auto accountElement = vbox({
            text(L"username  : " + to_wstring(mAccount.username())),
            text(L"points    : " + to_wstring(mAccount.points()))
        }) | xflex;

        auto doc = vbox({
            topBar,
            separator(),
            // mOpBar.Render(),
            hbox({
                mSearchInput.Render() | width(25), mSearchButton.Render(), 
                text(L" "), separator(), text(L" "),
                mCreateButton.Render(), 
                text(L" "), separator(), text(L" "),
                mQueryAccountInput.Render() | width(14), mQueryAccountButton.Render()
            }),
            separator(),
            mShowRoomList ? roomElement : accountElement
        });

        return doc | range(80, 25) | border | center;
    }

private:
    void SearchRoom() {
        mShowRoomList = true;
        // if search input is int
        /// TODO: query room
        // if search input is string
        auto ret = mUIProxy->ListRoom(to_string(mSearchInput.content));
        assert(ret.err() == ErrorNumber::OK);
        mRooms = Util::ConvertGrpcRepeatedPtrFieldToVector(ret.room());
        if (!mRooms.empty()) {
            mRoomList.selected = 0;
            QueryRoom(0);
        }
    }

    void CreateRoom() {
        auto ret = mUIProxy->CreateRoom();
        assert(ret.err() == ErrorNumber::OK);
        auto roomId = ret.roomid();
        // after creating room, user will join it automatically
        // auto joinRet =
        //     mUIProxy->JoinRoom(CoreMsgBuilder::CreateJoinRoomArgs(roomId));
        // assert(joinRet.err() == ErrorNumber::OK);
        OnJoinRoom(roomId);
        /// TODO: for now set game to UNO, in the future make it configurable
        auto ctrlRet = mUIProxy->ControlRoom(roomId, "UNO", "",
            Uno::MsgBuilder::CreateStartGameSettings(
                true, true, false, false, 15));
        spdlog::info("err: {}", ctrlRet.err());
        assert(ctrlRet.err() == ErrorNumber::OK);
    }

    void QueryAccount() {
        mShowRoomList = false;
        auto ret = mUIProxy->QueryAccount(to_string(mQueryAccountInput.content));
        assert(ret.err() == ErrorNumber::OK);
        mAccount = ret.account();
    }

    void QueryRoom(int roomIndex) {
        auto ret = mUIProxy->QueryRoom(mRooms[roomIndex].roomid());
        assert(ret.err() == ErrorNumber::OK);
        mDetails = ret.room();
    }

    void JoinRoom() {
        auto roomId = mRooms[mRoomList.selected].roomid();
        auto ret =
            mUIProxy->JoinRoom(roomId);
        assert(ret.err() == ErrorNumber::OK);
        OnJoinRoom(roomId);
    }

/// XXX: aggregate this states as one
public:
    std::string mUsername;
    int mPoints{0};
    std::vector<Room> mRooms;
    RoomDetails mDetails;
    // if true, left sidebar will show roomlist, otherwise show account info
    bool mShowRoomList{true};
    Account mAccount;

public:
    Container mContainer{Container::Vertical()};
    Container mOpBar{Container::Horizontal()};
    Input mSearchInput;
    NonBorderButton mSearchButton{L"Search Room"};
    NonBorderButton mCreateButton{L"Create Room"};
    Input mQueryAccountInput;
    NonBorderButton mQueryAccountButton{L"Query Account"};
    Container mRoomDisplayArea{Container::Horizontal()};
    Menu mRoomList;
    Container mRoomDetail{Container::Vertical()};
    TextCenterButton mJoinRoomButton{L"Join"};
};

}}