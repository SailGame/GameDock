#include "dock.hpp"

#include <ftxui/component/menu.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/screen/screen.hpp>
#include <thread>

#include "modeSelector.hpp"

namespace common {
namespace dock {

using namespace ftxui;

Dock::Dock(const DockConfig& config)
    : mDockConfig(config), mScreen(ScreenInteractive::Fullscreen()) {
  mCurrentElement = &mModeSelector;
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

}  // namespace dock
}  // namespace common