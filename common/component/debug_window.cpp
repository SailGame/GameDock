#include "debug_window.hpp"

#include <algorithm>
#include <ftxui/component/component.hpp>

namespace common {
namespace component {

using namespace ftxui;

DebugWindow::DebugWindow(unsigned int maxLines) : mMaxLines(maxLines) {}

Element DebugWindow::Render() {
  Elements elements;
  std::transform(mInfos.begin(), mInfos.end(),
                 std::inserter(elements, elements.begin()),
                 [](const std::wstring& debugStr) { return text(debugStr); });
  return vbox(elements);
}

void DebugWindow::DebugInfo(const std::wstring& info) {
  mInfos.emplace_back(info);
  if (mInfos.size() > mMaxLines) {
    mInfos.pop_front();
  }
}

}  // namespace component
}  // namespace common