#include <sailgame/uno/msg_builder.h>
#include <sailgame_pb/core/types.pb.h>
#include <ftxui/screen/string.hpp>
#include <google/protobuf/util/message_differencer.h>

#include "matcher.h"
#include "screen_fixture.h"

namespace SailGame { namespace Test {

using namespace testing;
using namespace Dock;
using Common::CoreMsgBuilder;
using ::Core::ErrorNumber;
using ftxui::to_string;
using ftxui::to_wstring;
using google::protobuf::util::MessageDifferencer;
using grpc::Status;

class LobbyScreenFixture : public ScreenFixture {
public:
    LobbyScreenFixture() : ScreenFixture() {
        LoginSuccess(mToken);
        mDock.mLobbyScreen.mUsername = "test";
        mDock.mLobbyScreen.mPoints = 2147483648;
        mDock.mLobbyScreen.TakeFocus();
    }

protected:
    std::string mToken{"tttokenforlobby"};
};

TEST_F(LobbyScreenFixture, SearchRoomList_HasRoom) {
    std::string content = "UNO";
    mDock.mLobbyScreen.mSearchInput.content = to_wstring(content);
    UserEvent();

    std::vector<Room> roomList = {
        CoreMsgBuilder::CreateRoom("UNO", 101, {"a", "b", "c"}),
        CoreMsgBuilder::CreateRoom("UNO", 102, {"at", "by", "co"}),
    };
    auto listRoomRet = CoreMsgBuilder::CreateListRoomRet(
        ErrorNumber::OK, roomList);
    EXPECT_CALL(*mMockStub, ListRoom(_, ListRoomArgsMatcher(content), _))
        .Times(1)
        .WillOnce(DoAll(SetArgPointee<2>(listRoomRet), Return(Status::OK)));

    auto queryRoomRet = CoreMsgBuilder::CreateQueryRoomRet(ErrorNumber::OK,
        CoreMsgBuilder::CreateRoomDetails("UNO", 101, {
            CoreMsgBuilder::CreateRoomUser("a", RoomUser::READY),
            CoreMsgBuilder::CreateRoomUser("b", RoomUser::READY),
            CoreMsgBuilder::CreateRoomUser("c", RoomUser::PREPARING),
        }, Uno::MsgBuilder::CreateStartGameSettings(
            true, true, false, false, 15)));
    EXPECT_CALL(*mMockStub, QueryRoom(_, QueryRoomArgsMatcher(mToken, 101), _))
        .Times(1)
        .WillOnce(DoAll(SetArgPointee<2>(queryRoomRet), Return(Status::OK)));

    mDock.mLobbyScreen.mSearchButton.TakeFocus();
    UserEvent(mDock.mLobbyScreen.mSearchButton.on_click);
    for (auto i = 0; i < roomList.size(); i++) {
        EXPECT_TRUE(MessageDifferencer::Equals(mDock.mLobbyScreen.mRooms[i],
            roomList[i]));
    }
    EXPECT_TRUE(MessageDifferencer::Equals(mDock.mLobbyScreen.mDetails,
        queryRoomRet.room()));
}

TEST_F(LobbyScreenFixture, SearchRoomList_NoRoom) {
    std::string content = "UNA";
    mDock.mLobbyScreen.mSearchInput.content = to_wstring(content);
    UserEvent();

    auto listRoomRet = CoreMsgBuilder::CreateListRoomRet(ErrorNumber::OK, {});
    EXPECT_CALL(*mMockStub, ListRoom(_, ListRoomArgsMatcher(content), _))
        .Times(1)
        .WillOnce(DoAll(SetArgPointee<2>(listRoomRet), Return(Status::OK)));
    EXPECT_CALL(*mMockStub, QueryRoom(_, _, _)).Times(0);
    
    mDock.mLobbyScreen.mSearchButton.TakeFocus();
    UserEvent(mDock.mLobbyScreen.mSearchButton.on_click);
    EXPECT_TRUE(mDock.mLobbyScreen.mRooms.empty());
}

}}