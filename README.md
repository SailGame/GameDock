# GameDock

GameDock - Dock all games which in your mind

## Build instructions

```bash
git submodule update --init --recursive
mkdir build
cd build
cmake ..
make
./main
```

```bash
# clang-format
# version requirement >10
python3 run-clang-format.py -i -r .
```

```bash
# function key
F1 exit current state
F2 list room
F3 create room
F4 set room
F5 ready
```