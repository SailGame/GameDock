#pragma once

#include <ftxui/component/component.hpp>

#include "hook.hpp"

namespace common {
namespace game {

class GameClient {
  virtual ftxui::Component* UIComponent() = 0;
  virtual void Start() = 0;
  virtual void Stop() = 0;
  virtual void HandleServerMsg(/*TODO*/) = 0;
  virtual void SetPlugin(Plugin*) = 0;
  virtual bool SupportsOnlineMode() = 0;
  virtual bool SupportsOfflineMode() = 0;
};

}  // namespace game
}  // namespace common
