#include <common/dock/dock.hpp>
#include <iostream>

int main(int argc, const char* argv[]) {
  using namespace common::dock;

  // TODO: read from input/yaml/xml/json ?
  DockConfig dockConfig;
  Dock dock(dockConfig);
  dock.Loop();
}