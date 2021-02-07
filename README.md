# GameDock

GameDock - Dock all games which in your mind

## Build instructions

```bash
git submodule update --init --recursive
mkdir build
cd build
cmake ..
cmake --build . --target sailgame
./sailgame
```

```bash
# clang-format
# version requirement >10
python3 run-clang-format.py -i -r .
```