
#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/component/menu.hpp>
#include <ftxui/screen/screen.hpp>

namespace common {
namespace dock {

class ModeSelector : public ftxui::Component {
 public:
  ModeSelector();
  ftxui::Element Render() override;
  virtual bool OnEvent(ftxui::Event e) override { return mMenu.OnEvent(e); }

 private:
  void OnEnter();

 private:
  ftxui::Menu mMenu;
};

}  // namespace dock
}  // namespace common
