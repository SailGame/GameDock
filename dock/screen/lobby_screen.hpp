#pragma once

#include <sailgame/common/util.h>
#include <sailgame_pb/core/error.pb.h>
#include <sailgame_pb/core/types.pb.h>

#include <ftxui/component/button.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/container.hpp>
#include <ftxui/component/input.hpp>
#include <ftxui/component/menu.hpp>
#include <ftxui/screen/string.hpp>

#include "../component/non_border_button.hpp"
#include "../component/text_center_button.hpp"
#include "../core/component.h"
#include "../util/dom.hpp"
#include "../util/util.hpp"

namespace SailGame { namespace Dock {

using namespace ftxui;

using ::Core::ErrorNumber;
using ::Core::JoinRoomRet;
using ::Core::Room;
using ::Core::RoomDetails;
using SailGame::Common::CoreMsgBuilder;
using SailGame::Common::Util;
using SailGame::Dock::DockUtil;
using SailGame::Dock::NonBorderButton;

class LobbyScreen : public DockComponent {
public:
    std::function<void(int)> OnJoinRoom;

    LobbyScreen() {
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

        mRoomList.on_change = [this] { QueryRoomByIndex(mRoomList.selected); };

        mJoinRoomButton.on_click = [this] { JoinRoom(); };
    }

    void Update() {
        Dom::MapVectorToMenuEntries(mRoomList, mRooms, &Dom::RoomToString);
        if (mRooms.empty() && mRoomList.Focused()) {
            if (mDetails.gamename().empty()) {
                mContainer.SetActiveChild(&mOpBar);
            }
            else {
                mRoomDisplayArea.SetActiveChild(&mRoomDetail);
            }
        }
    }

    Element Render() final {
        Update();

        auto topBar =
            hbox({text(L"username: "), text(to_wstring(mUsername)), filler(),
                  text(L"points: "), text(to_wstring(mPoints))});

        auto roomList =
            vbox({text(L"Room List"), separator(), mRoomList.Render()}) | flex |
            width(20);

        auto roomDetail =
            vbox({text(L"Room Detail"), separator(),
                  hbox({
                      text(L"Game: ") | bold,
                      text(to_wstring(mDetails.gamename())) | flex,
                      text(L"RoomId: ") | bold,
                      text(to_wstring(mDetails.roomid())) | flex,
                  }),
                  separator(), text(L"Players in the room") | bold,
                  Dom::MapVectorToVBox(
                      DockUtil::GetUsernamesFromRoomDetails(mDetails),
                      &Dom::UserNameToText),
                  separator(), text(L"Game settings") | bold,
                  Dom::ShowGameSettings(mDetails), separator(),
                  mJoinRoomButton.Render()}) |
            xflex;

        auto roomElement = hbox({roomList, separator(),
                                 (mDetails.gamename().empty()
                                      ? text(L"choose a room to view details")
                                      : roomDetail)}) |
                           yflex;

        auto accountElement =
            // vbox({text(L"username  : " + to_wstring(mAccount.username())),
            //       text(L"points    : " + to_wstring(mAccount.points()))}) |
            // xflex;
            text(L"QueryAccount not supported yet");

        auto doc = vbox(
            {topBar, separator(),
             // mOpBar.Render(),
             hbox({mSearchInput.Render() | width(25), mSearchButton.Render(),
                   text(L" "), separator(), text(L" "), mCreateButton.Render(),
                   text(L" "), separator(), text(L" "),
                   mQueryAccountInput.Render() | width(14),
                   mQueryAccountButton.Render()}),
             separator(), mShowRoomList ? roomElement : accountElement});

        return TryRenderDialog(doc) | range(80, 25) | border | center;
    }

private:
    void SearchRoom() {
        if (mSearchInput.content.empty()) {
            return;
        }
        mShowRoomList = true;
        auto content = to_string(mSearchInput.content);
        if (auto roomId = DockUtil::TryToGetRoomId(content); roomId != -1) {
            QueryRoomByRoomId(roomId);
        } else {
            auto ret = mUIProxy->ListRoom(content);
            if (ret.err() != ErrorNumber::OK) {
                ShowDialogWithText();
                return;
            }
            mRooms = Util::ConvertGrpcRepeatedPtrFieldToVector(ret.room());
            if (!mRooms.empty()) {
                mRoomList.selected = 0;
                QueryRoomByIndex(0);
            }
        }
    }

    void CreateRoom() {
        auto ret = mUIProxy->CreateRoom();
        if (ret.err() != ErrorNumber::OK) {
            ShowDialogWithText();
            return;
        }
        auto roomId = ret.roomid();

        /// TODO: for now set game to UNO, in the future make it configurable
        auto ctrlRet =
            mUIProxy->ControlRoom(roomId, "UNO", "",
                                  Uno::MsgBuilder::CreateStartGameSettings(
                                      true, true, false, false, 15));
        if (ctrlRet.err() != ErrorNumber::OK) {
            ShowDialogWithText();
            return;
        }

        auto joinRet = mUIProxy->JoinRoom(roomId);
        if (joinRet.err() != ErrorNumber::OK) {
            ShowDialogWithText();
            return;
        }

        OnJoinRoom(roomId);
    }

    void QueryAccount() {
        mShowRoomList = false;
        /// XXX: QueryAccount hasn't been supported completely by Core
        // auto ret =
        //     mUIProxy->QueryAccount(to_string(mQueryAccountInput.content));
        // assert(ret.err() == ErrorNumber::OK);
        // mAccount = ret.account();
    }

    void QueryRoomByRoomId(int roomId) {
        auto ret = mUIProxy->QueryRoom(roomId);
        if (ret.err() != ErrorNumber::OK) {
            switch (ret.err()) {
                case ErrorNumber::QryRoom_InvalidRoomID:
                    ShowDialogWithText("Room doesn't exist.");
                    return;
                default:
                    ShowDialogWithText();
                    return;
            }
        }
        mDetails = ret.room();
    }

    void QueryRoomByIndex(int roomIndex) {
        QueryRoomByRoomId(mRooms[roomIndex].roomid());
    }

    void JoinRoom() {
        // auto roomId = mRooms[mRoomList.selected].roomid();
        auto roomId = mDetails.roomid();
        auto ret = mUIProxy->JoinRoom(roomId);
        if (ret.err() != ErrorNumber::OK) {
            switch (ret.err()) {
                case ErrorNumber::JoinRoom_FullRoom:
                    ShowDialogWithText("Room is full.");
                    return;
                default:
                    ShowDialogWithText();
                    return;
            }
        }
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

}}  // namespace SailGame::Dock