#include <chrono>
#include <ftxui/component/menu.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <iostream>
#include <tetris/tetrisBox.hpp>
#include <thread>

using namespace tetris;

int main(int argc, const char* argv[]) {
  using namespace ftxui;
  auto screen = ScreenInteractive::TerminalOutput();

  TetrisBox menu;
  menu.entries = {L"entry 1", L"entry 2", L"entry 3"};
  menu.selected = 0;
  menu.on_enter = screen.ExitLoopClosure();

  screen.Loop(&menu);

  std::cout << "Selected element = " << menu.selected << std::endl;
}