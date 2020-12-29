#pragma once

#include <common/component/debug_window.hpp>
#include <common/game/client.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <iostream>
#include <vector>

#include "config.hpp"
#include "lobby.hpp"
#include "state.hpp"

namespace common {
namespace dock {

class Dock : public ftxui::Component {
 public:
  Dock(const DockConfig&);
  ~Dock();

  // FtxUI
  void Loop();
  ftxui::Element Render() override;
  virtual bool OnEvent(ftxui::Event e) override;

 private:
  bool OnFunctionEvent(ftxui::Event e);

  DockConfig mDockConfig;
  ftxui::ScreenInteractive mScreen;
  DockFSM mDockFSM;
  dock::Lobby mLobby;
  component::DebugWindow mDebugWindow;
  // Room mRoom;
  // Playing mPlaying;
  ftxui::Component* mCurrentElement;
  std::vector<game::GameClient*> mGameClients;
};

}  // namespace dock
}  // namespace common