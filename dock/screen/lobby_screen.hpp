#pragma once

#include <sailgame/common/util.h>
#include <sailgame_pb/core/error.pb.h>
#include <sailgame_pb/core/types.pb.h>

#include <ftxui/component/component.hpp>
#include <ftxui/screen/string.hpp>

#include "../core/component.h"
#include "../dock/component/option.hpp"
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

class LobbyScreen : public DockComponent {
public:
    std::function<void(int)> OnJoinRoom;

    LobbyScreen() {
        auto roomListOption = MenuOption::Vertical();
        roomListOption.on_change = [this] {
            if (mRoomSelected < mRooms.size()) QueryRoomByIndex(mRoomSelected);
        };
        mRoomList = Menu(&mRoomNames, &mRoomSelected, roomListOption);

        mJoinRoomButton = Button(
            L"Join", [this] { JoinRoom(); }, TextCenterButtonOption());

        mRoomDetail = Container::Vertical({mJoinRoomButton});

        mRoomDisplayArea = Container::Horizontal({mRoomList, mRoomDetail});
        mSearchInput = Input(&mSearchInputContent, L"game name or room id");
        mSearchButton = Button(
            L"Search Room", [this] { SearchRoom(); }, NonBorderButtonOption());

        mCreateButton = Button(
            L"Create Room", [this] { CreateRoom(); }, NonBorderButtonOption());

        mQueryAccountInput = Input(&mQueryAccountInputContent, L"username");
        mQueryAccountButton = Button(
            L"Query Account", [this] { QueryAccount(); },
            NonBorderButtonOption());

        mOpBar =
            Container::Horizontal({mSearchInput, mSearchButton, mCreateButton,
                                   mQueryAccountInput, mQueryAccountButton});

        mContainer->Add(mOpBar);
        mContainer->Add(mRoomDisplayArea);
    }

    void Update() {
        mRoomNames.clear();
        for (const auto& room : mRooms) {
            mRoomNames.push_back(Dom::RoomToString(room));
        }
        if (mRooms.empty() && mRoomList->Focused()) {
            if (mDetails.gamename().empty()) {
                mContainer->SetActiveChild(mOpBar.get());
            } else {
                mRoomDisplayArea->SetActiveChild(mRoomDetail.get());
            }
        }
    }

    Element Render() final {
        Update();

        auto topBar =
            hbox({text(L"username: "), text(to_wstring(mUsername)), filler(),
                  text(L"points: "), text(to_wstring(mPoints))});

        auto roomList =
            vbox({text(L"Room List"), separator(), mRoomList->Render()}) |
            flex | width(20);

        auto roomDetail =
            vbox({text(L"Room Detail"), separator(),
                  hbox({
                      text(L"Game: ") | bold,
                      text(to_wstring(mDetails.gamename())) | flex,
                      text(L"RoomId: ") | bold,
                      text(to_wstring(mDetails.roomid())) | flex,
                  }),
                  separator(), text(L"Players in the room: ") | bold,
                  Dom::MapVectorToVBox(
                      DockUtil::GetUsernamesFromRoomDetails(mDetails),
                      &Dom::UserNameToText),
                  separator(), text(L"Game settings: ") | bold,
                  Dom::ShowGameSettings(mDetails), separator(),
                  mJoinRoomButton->Render()}) |
            xflex;

        auto roomElement = hbox({roomList, separator(),
                                 (mDetails.roomid() == 0
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
             hbox({mSearchInput->Render() | width(25), mSearchButton->Render(),
                   text(L" "), separator(), text(L" "), mCreateButton->Render(),
                   text(L" "), separator(), text(L" "),
                   mQueryAccountInput->Render() | width(14),
                   mQueryAccountButton->Render()}),
             separator(), mShowRoomList ? roomElement : accountElement});

        return TryRenderDialog(doc) | range(80, 25) | border | center;
    }

private:
    void SearchRoom() {
        mShowRoomList = true;
        auto content = mSearchInputContent;
        if (auto roomId = DockUtil::TryToGetRoomId(content); roomId != -1) {
            QueryRoomByRoomId(roomId);
        } else {
            auto ret = mUIProxy->ListRoom(content);
            if (ret.err() != ErrorNumber::OK) {
                ShowDialogWithText(ErrorNumber_Name(ret.err()));
                return;
            }
            mRooms = Util::ConvertGrpcRepeatedPtrFieldToVector(ret.room());
            if (!mRooms.empty()) {
                mRoomSelected = 0;
                QueryRoomByIndex(mRoomSelected);
            }
        }
    }

    void CreateRoom() {
        auto ret = mUIProxy->CreateRoom();
        if (ret.err() != ErrorNumber::OK) {
            ShowDialogWithText(ErrorNumber_Name(ret.err()));
            return;
        }
        auto roomId = ret.roomid();

        auto joinRet = mUIProxy->JoinRoom(roomId);
        if (joinRet.err() != ErrorNumber::OK) {
            ShowDialogWithText(ErrorNumber_Name(joinRet.err()));
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
            ShowDialogWithText(ErrorNumber_Name(ret.err()));
            mDetails.Clear();
            return;
        }
        mDetails = ret.room();
    }

    void QueryRoomByIndex(int roomIndex) {
        QueryRoomByRoomId(mRooms.at(roomIndex).roomid());
    }

    void JoinRoom() {
        auto roomId = mDetails.roomid();
        auto ret = mUIProxy->JoinRoom(roomId);
        if (ret.err() != ErrorNumber::OK) {
            ShowDialogWithText(ErrorNumber_Name(ret.err()));
            return;
        }
        OnJoinRoom(roomId);
    }

    /// XXX: aggregate this states as one
public:
    std::string mUsername;
    int mPoints{0};
    std::vector<Room> mRooms;
    std::vector<std::wstring> mRoomNames;
    RoomDetails mDetails;
    // if true, left sidebar will show roomlist, otherwise show account info
    bool mShowRoomList{true};
    Account mAccount;

public:
    std::string mSearchInputContent;
    std::string mQueryAccountInputContent;
    int mRoomSelected = 0;
    ftxui::Component mContent;
    ftxui::Component mOpBar;
    ftxui::Component mSearchInput;
    ftxui::Component mSearchButton;
    ftxui::Component mCreateButton;
    ftxui::Component mQueryAccountInput;
    ftxui::Component mQueryAccountButton;
    ftxui::Component mRoomDisplayArea;
    ftxui::Component mRoomList;
    ftxui::Component mRoomDetail;
    ftxui::Component mJoinRoomButton;
};

}}  // namespace SailGame::Dock