#include "modeSelector.hpp"

#include <ftxui/component/component.hpp>

namespace common {
namespace dock {

using namespace ftxui;

ModeSelector::ModeSelector() {
  mMenu.entries = {L"Online", L"Offline"};
  mMenu.selected = 0;
  mMenu.on_enter = std::bind(&ModeSelector::OnEnter, this);
}

Element ModeSelector::Render() { return vbox({mMenu.Render() | hcenter}); }

void ModeSelector::OnEnter() {}

}  // namespace dock
}  // namespace common