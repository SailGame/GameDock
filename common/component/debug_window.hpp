#pragma once

#include <string>
#include <ftxui/component/component.hpp>
#include <ftxui/screen/screen.hpp>

namespace common {
namespace component {

class DebugWindow : public ftxui::Component {
 public:
  explicit DebugWindow(unsigned int maxLines);
  ftxui::Element Render() override;
  void DebugInfo(const std::wstring&);

 private:
  void OnEnter();

 private:
  std::deque<std::wstring> mInfos;
  const int mMaxLines;
};

}  // namespace component
}  // namespace common
