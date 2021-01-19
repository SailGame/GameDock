#include <iostream>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <sailgame/common/game_manager.h>

#include "dock/dock.h"

using SailGame::Common::EventLoop;
using SailGame::Common::INetworkInterface;
using SailGame::Common::ClientNetworkInterface;
using SailGame::Dock::Dock;
using SailGame::Dock::UIProxy;

int main(int argc, const char* argv[]) {
    spdlog::set_default_logger(spdlog::basic_logger_mt("GameDock", "dev.log"));
    spdlog::set_level(spdlog::level::info);
    spdlog::default_logger()->flush_on(spdlog::level::info);
    spdlog::info("hello, game dock!");

    auto uiProxy = UIProxy::Create(
        ClientNetworkInterface::Create(
            INetworkInterface::CreateStub("localhost:50051")));
    Dock dock(uiProxy);
    dock.Loop();
}