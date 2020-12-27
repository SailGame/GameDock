#pragma once

#include <ftxui/component/component.hpp>
#include <string>

namespace common {
namespace game {

struct GameInfo {
  std::string GameName;
  bool SupportsOnlineMode = false;
  bool SupportsOfflineMode = false;
};

class GameClient {
  virtual ftxui::Component* UIComponent() = 0;
  virtual GameInfo& GetGameInfo() const = 0;
  virtual void Start() = 0;
  virtual void Stop() = 0;
  virtual void HandleServerMsg(/*TODO*/) = 0;
};

}  // namespace game
}  // namespace common
