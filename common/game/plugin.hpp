#pragma once

#include <common/archive/archive.hpp>

namespace common {
namespace game {

class Plugin {
  virtual void SendGameMsg() = 0;
  virtual void SaveArchive(const archive::Archive&) = 0;
};

}  // namespace game
}  // namespace common
