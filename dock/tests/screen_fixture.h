#pragma once

#include <gtest/gtest.h>
#include <grpcpp/test/mock_stream.h>

#include <sailgame_pb/core/types.pb.h>
#include <sailgame_pb/core/core_mock.grpc.pb.h>
#include <ftxui/screen/string.hpp>

#include "../dock.h"

namespace SailGame { namespace Test {

using namespace testing;
using namespace Dock;
using Common::CoreMsgBuilder;
using Common::NetworkInterface;
using ::Core::BroadcastMsg;
using ::Core::ErrorNumber;
using ::Core::MockGameCoreStub;
using ftxui::to_string;
using ftxui::to_wstring;
using grpc::Status;
using grpc::testing::MockClientReader;

class ScreenFixture : public ::testing::Test {
public:
    ScreenFixture() 
        : mMockStream(new MockClientReader<BroadcastMsg>()),
        mMockStub(std::make_shared<MockGameCoreStub>()),
        mDock(UIProxy::Create(
            NetworkInterface<false>::Create(mMockStub), true))
    {}

    void SetUp() {
        spdlog::set_level(spdlog::level::err);
        mThread = std::make_unique<std::thread>([&] {
            mDock.Loop();
        });
        using namespace std::chrono_literals;
        // make sure mDock has entered loop before running test case
        std::this_thread::sleep_for(10ms);
    }

    void TearDown() {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1s);
        mDock.mScreen.ExitLoopClosure()();
        mDock.mUIProxy->Stop();
        mThread->join();
    }

    void UserEvent(const std::function<void()> &callback) {
        // wait for 1 seconds to display ui
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1s);
        callback();
        // an event is needed to refresh ui, i don't know why
        mDock.mScreen.PostEvent(ftxui::Event::Custom);
    }

protected:
    MockClientReader<BroadcastMsg> *mMockStream;
    std::shared_ptr<MockGameCoreStub> mMockStub;

    Dock::Dock mDock;
    std::unique_ptr<std::thread> mThread;
};

}}
