#pragma once

#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <iostream>

#include "config.hpp"
#include "modeSelector.hpp"

namespace common {
namespace dock {

class Dock : public ftxui::Component {
 public:
  Dock(const DockConfig&);
  ~Dock();

  void Loop();

  ftxui::Element Render() override { return mCurrentElement->Render(); }

  virtual bool OnEvent(ftxui::Event e) {
    // TODO: use F1~F12 as functional key
    // e.g. exit game, list room, xxx
    // pass other event to downstream
    return mCurrentElement->OnEvent(e);
  }

 private:
  DockConfig mDockConfig;
  ftxui::ScreenInteractive mScreen;
  ModeSelector mModeSelector;

  ftxui::Component* mCurrentElement;
};

}  // namespace dock
}  // namespace common