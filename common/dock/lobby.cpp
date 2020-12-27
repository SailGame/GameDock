#include "lobby.hpp"

#include <ftxui/component/component.hpp>

namespace common {
namespace dock {

using namespace ftxui;

Lobby::Lobby() {
  mMenu.entries = {L"Online", L"Offline"};
  mMenu.selected = 0;
  mMenu.on_enter = std::bind(&Lobby::OnEnter, this);
}

Element Lobby::Render() { return vbox({mMenu.Render() | hcenter}); }

bool Lobby::OnEvent(ftxui::Event e) { return mMenu.OnEvent(e); }

void Lobby::OnEnter() {}

}  // namespace dock
}  // namespace common