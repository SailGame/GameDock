
#pragma once

namespace common {
namespace dock {

class DockHandler {
 public:
  DockHandler();

  virtual void ListRoom() = 0;
  virtual void CreateRoom()
};

}  // namespace dock
}  // namespace common
