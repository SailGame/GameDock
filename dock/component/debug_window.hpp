#pragma once

#include <algorithm>
#include <ftxui/component/component.hpp>
#include <ftxui/screen/screen.hpp>
#include <string>

namespace SailGame { namespace Dock {

using namespace ftxui;

class DebugWindow : public ftxui::Component {
public:
    explicit DebugWindow(unsigned int maxLines) : mMaxLines(maxLines) {}

    ftxui::Element Render() override {
        Elements elements;
        std::transform(
            mInfos.begin(), mInfos.end(),
            std::inserter(elements, elements.begin()),
            [](const std::wstring& debugStr) { return text(debugStr); });
        return vbox(elements);
    }

    void DebugInfo(const std::wstring& info) {
        mInfos.emplace_back(info);
        if (mInfos.size() > mMaxLines) {
            mInfos.pop_front();
        }
    }

private:
    void OnEnter();

private:
    std::deque<std::wstring> mInfos;
    const int mMaxLines;
};

}}  // namespace SailGame::Dock
