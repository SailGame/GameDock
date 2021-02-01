#include <sailgame/common/game_manager.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include <cxxopts.hpp>
#include <iostream>

#include "dock/dock.h"

using SailGame::Common::ClientNetworkInterface;
using SailGame::Common::EventLoop;
using SailGame::Common::INetworkInterface;
using SailGame::Dock::Dock;
using SailGame::Dock::UIProxy;

int main(int argc, const char* argv[]) {
    cxxopts::Options options("SailGame", "Terminal Game Platform");

    options.add_options()("c, core", "endpoint of core",
                          cxxopts::value<std::string>());

    try {
        auto results = options.parse(argc, argv);
        auto coreAddress = results["core"].as<std::string>();

        spdlog::set_default_logger(
            spdlog::basic_logger_mt("GameDock", "dev.log"));
        spdlog::set_level(spdlog::level::info);
        spdlog::default_logger()->flush_on(spdlog::level::info);
        spdlog::info("hello, game dock!");

        auto uiProxy = UIProxy::Create(ClientNetworkInterface::Create(
            INetworkInterface::CreateStub(coreAddress)));
        Dock dock(uiProxy);
        dock.Loop();
    } catch (std::exception& e) {
        std::cout << options.help() << std::endl;
        std::cout << e.what() << std::endl;
        std::exit(-1);
    }
}