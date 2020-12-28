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
    : mDockConfig(config), mScreen(ScreenInteractive::Fullscreen()), mDebugWindow(10) {
      (void) mDockConfig;
      Add(&mLobby);
      Add(&mDebugWindow);
      SetActiveChild(&mLobby);
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

  mDebugWindow.DebugInfo(L"Dock Started");
  mScreen.Loop(this);
}

Element Dock::Render() { 
  return hbox({
    mCurrentElement->Render() | border | flex,
    vbox({
      vbox({text(L"Game Info")}) | border,
      mDebugWindow.Render() | border,
    }) | border | flex
  });
}

bool Dock::OnEvent(ftxui::Event e) {
  // TODO: use F1~F12 as functional key
  // e.g. exit game, list room, xxx
  // pass other event to downstream
  if(e == Event::Custom)
  {
    // refresh
    return true;
  }
  mDebugWindow.DebugInfo(std::wstring(L"OnEvent: ")+e.character());

  if(OnFunctionEvent(e))
  {
    return true;
  }
  return mCurrentElement->OnEvent(e);
}

bool Dock::OnFunctionEvent(ftxui::Event e)
{
  if (e == Event::F1)
  {

  }
  else if (e == Event::F2)
  {
    
  }
  else if (e == Event::F3)
  {
    
  }
  else if (e == Event::F4)
  {
    
  }
  else if (e == Event::F5)
  {
    
  }
  else
  {
    return false;
  }
  return true;
}

}  // namespace dock
}  // namespace common