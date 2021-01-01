
#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/component/menu.hpp>
#include <ftxui/screen/screen.hpp>

namespace common {
namespace dock {

class Lobby : public ftxui::Component {
 public:
  Lobby();
  ftxui::Element Render() override;
  bool OnEvent(ftxui::Event e) override;

 private:
  void OnEnter();

 private:
  ftxui::Menu mRooms;
};

}  // namespace dock
}  // namespace common
