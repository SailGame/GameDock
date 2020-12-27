#include "dock.hpp"

#include <ftxui/component/menu.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/screen/screen.hpp>
#include <thread>

#include "lobby.hpp"

namespace common {
namespace dock {

using namespace ftxui;

Dock::Dock(const DockConfig& config)
    : mDockConfig(config), mScreen(ScreenInteractive::Fullscreen()) {
  mCurrentElement = &mLobby;
}
Dock::~Dock() {}

void Dock::Loop() {
  std::thread update([&]() {
    for (;;) {
      using namespace std::chrono_literals;
      std::this_thread::sleep_for(0.05s);
      mScreen.PostEvent(Event::Custom);
    }
  });

  mScreen.Loop(this);
}

Element Dock::Render() { return mCurrentElement->Render(); }

bool Dock::OnEvent(ftxui::Event e) {
  // TODO: use F1~F12 as functional key
  // e.g. exit game, list room, xxx
  // pass other event to downstream
  return mCurrentElement->OnEvent(e);
}

}  // namespace dock
}  // namespace common