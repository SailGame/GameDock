#include <functional>

#include "ftxui/component/component.hpp"
#include "ftxui/dom/elements.hpp"

using namespace ftxui;

namespace tetris {

class TetrisBox : public Component {
public:
    // Constructor.
    TetrisBox() = default;
    ~TetrisBox() override = default;

    // State.
    std::vector<std::wstring> entries = {};
    int selected = 0;

    Decorator focused_style = inverted;
    Decorator selected_style = bold;
    Decorator normal_style = nothing;

    // State update callback.
    std::function<void()> on_change = []() {};
    std::function<void()> on_enter = []() {};

    // Component implementation.
    Element Render() override;
    bool OnEvent(Event) override;
};

}  // namespace tetris