#include <gtest/gtest.h>
#include "screen_fixture.h"

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

namespace SailGame { namespace Test {

class InteractiveUIFixture : public ScreenFixture {
public:
    InteractiveUIFixture() = default;

    void SetUp() {
        ScreenFixture::SetUp();

        // Login
        auto loginRet = CoreMsgBuilder::CreateLoginRet(ErrorNumber::OK, "tkn", 
            CoreMsgBuilder::CreateAccount("tbc", 147));
        EXPECT_CALL(*mMockStub, Login(_, _, _))
            .Times(AnyNumber())
            .WillRepeatedly(DoAll(
                SetArgPointee<2>(loginRet), 
                Return(Status::OK)
            ));

        // Listen
        EXPECT_CALL(*mMockStub, ListenRaw(_, _))
            .Times(AnyNumber())
            .WillRepeatedly(Return(mMockStream));

        // QueryAccount

        // CreateRoom

        // ControlRoom

        // ListRoom
        std::vector<Room> roomList = {
            CoreMsgBuilder::CreateRoom("UNO", 101, {"a", "b", "c"}),
            CoreMsgBuilder::CreateRoom("UNO", 102, {"at", "by", "co"}),
        };
        auto listRoomRet = CoreMsgBuilder::CreateListRoomRet(
            ErrorNumber::OK, roomList);
        EXPECT_CALL(*mMockStub, ListRoom(_, _, _))
            .Times(AnyNumber())
            .WillRepeatedly(DoAll(SetArgPointee<2>(listRoomRet), Return(Status::OK)));

        // JoinRoom

        // ExitRoom
        auto exitRoomRet = CoreMsgBuilder::CreateExitRoomRet(ErrorNumber::OK);
        EXPECT_CALL(*mMockStub, ExitRoom(_, _, _))
            .Times(AnyNumber())
            .WillRepeatedly(DoAll(SetArgPointee<2>(exitRoomRet), Return(Status::OK)));

        // QueryRoom
        auto roomDetails = CoreMsgBuilder::CreateRoomDetails("UNO", 101, {
            CoreMsgBuilder::CreateRoomUser("a", RoomUser::READY),
            CoreMsgBuilder::CreateRoomUser("b", RoomUser::READY),
            CoreMsgBuilder::CreateRoomUser("c", RoomUser::PREPARING),
        }, Uno::MsgBuilder::CreateStartGameSettings(
            true, true, false, false, 15));
        auto queryRoomRet = CoreMsgBuilder::CreateQueryRoomRet(ErrorNumber::OK,
            roomDetails);
        EXPECT_CALL(*mMockStub, QueryRoom(_, _, _))
            .Times(AnyNumber())
            .WillRepeatedly(DoAll(
                // Return should be the last action of DoAll
                SetArgPointee<2>(queryRoomRet), 
                [this, roomDetails] {
                    CoreMsg(
                        CoreMsgBuilder::CreateBroadcastMsgByRoomDetails(
                            0, 0, 0, roomDetails)); },
                Return(Status::OK)
            ));

        // OperationInRoom
        auto opRet = CoreMsgBuilder::CreateOperationInRoomRet(ErrorNumber::OK);
        EXPECT_CALL(*mMockStub, OperationInRoom(_, _, _))
            .Times(AnyNumber())
            .WillRepeatedly(DoAll(SetArgPointee<2>(opRet), Return(Status::OK)));

        // Message

        // start refresher
        mTimerThread = std::make_unique<std::thread>([this] {
            for (;;) {
                using namespace std::chrono_literals;
                std::this_thread::sleep_for(0.05s);
                mDock.mScreen.PostEvent(ftxui::Event::Custom);
            }
        });
    }

    void TearDown() {
        mTimerThread->join();
        ScreenFixture::TearDown();
    }

    std::unique_ptr<std::thread> mTimerThread;
};

TEST_F(InteractiveUIFixture, Run) {
    /// TODO: add an Exit button
    while (true) {}
}

}}