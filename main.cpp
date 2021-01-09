#include <iostream>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <sailgame/common/game_manager.h>

#include "dock/dock.h"

using SailGame::Common::EventLoop;
using SailGame::Common::GameManager;
using SailGame::Common::NetworkInterface;
using SailGame::Dock::Dock;

int main(int argc, const char* argv[]) {
  spdlog::set_default_logger(spdlog::basic_logger_mt("GameDock", "dev.log"));
  spdlog::set_level(spdlog::level::info);
  spdlog::default_logger()->flush_on(spdlog::level::info);
  spdlog::info("hello, game dock!");

  Dock dock;

  dock.Loop();
}