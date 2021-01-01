#include "lobby.hpp"

#include <ftxui/component/component.hpp>

#include "dock.hpp"

namespace common {
namespace dock {

using namespace ftxui;

Lobby::Lobby() {
  mRooms.entries = {L"Room1", L"Room2"};
  mRooms.selected = 0;
  mRooms.on_enter = std::bind(&Lobby::OnEnter, this);
}

Element Lobby::Render() { return vbox({mRooms.Render() | vcenter}); }

bool Lobby::OnEvent(ftxui::Event e) { return mRooms.OnEvent(e); }

void Lobby::OnEnter() {
  if (Focused()) {
  }
}

}  // namespace dock
}  // namespace common