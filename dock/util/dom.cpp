#include "dom.hpp"

namespace ftxui {

Decorator range(int width, int height) {
    return size(WIDTH, EQUAL, width) | size(HEIGHT, EQUAL, height);
}

Decorator width(int value) { return size(WIDTH, EQUAL, value); }

Decorator height(int value) { return size(HEIGHT, EQUAL, value); }

}  // namespace ftxui