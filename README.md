# Pathfinding demo

![C++ pathfinding demo](./docs/img/screenshot_1.png)

**Work in progress**

This is a demo of pathfinding on a 2D grid. It consists of 2 main parts:

* python notes and implementation
    * jupyter notebook file
    * standalone python script
* C++ interactive demo

## Python

### Standalone script

Run `python pathfinding_demo.py`. Requires numpy and matplotlib to be installed.

### Jupyter notebook

Contains the same demo as the standalone script and some notes. Since Github supports Jupyter Notebooks now, you can go [directly to the file](./python/pathfinding_demo.ipynb).

## C++

Work in progress. At the moment Linux build only.

### Build

#### Install prerequisities

* SDL3
* SDL3-image
* GLEW

e.g. on Archlinux:

```
pacman -S glew sdl3 sdl3_image
```

#### Compile

In the [cpp](./cpp/) folder, run [make](./cpp/Makefile)

```
cd cpp/
make -j $(nproc)
```

### Run

Run the `pathfinding` binary in the [cpp](./cpp/) folder.

## TODO

- [x] python
    - [x] get jupyter lab running
    - [x] drawing utility
    - [x] interface for pathfinding
    - [x] research methods
    - [x] implement methods
        - [x] DFS
        - [x] BFS
        - [x] Dijsktra
        - [x] GBFS
        - [x] A*
    - [x] performance measurement: time/visited nodes
    - [x] finalize the script and copy back to the jupyter notebook
    - [x] finish text on the page
    - [x] create a dedicated python script
- [ ] C++
    - [x] re-use 2D game engine
    - [x] add tiles (with cost) to map
        - [x] conversion functions from tile coords to world coords
        - [x] drawing tiles
        - [x] add "terrain tiles" with different costs
    - [x] add mouse-click action
    - [x] add direct movement (through mouse click action, no pathfinding)
    - [x] implement pathfinding
        - [x] BFS
        - [x] GBFS
        - [x] Dijkstra
        - [ ] A*
    - [ ] windows build?
        - [x] VS solution
        - [ ] merge to master
        - [ ] cmake?
    - [x] add screenshot
    - [ ] zoom + pan of the map
    - [ ] maze generator?
    - [ ] collisions
    - [ ] multiple units
        - change from single unit (player) to RTS-style multiple units
    - [ ] unit selection
        - selection rectangle?
    - [ ] pathfinding for multiple units
    - [ ] pathfinding for multiple units
        - group formation, local cohesion, etc
    - [ ] cpython interface
        - control the game through the interpreter
    - [ ] clang-format config
    - [ ] git hooks?
    - [ ] [gcovr](https://gcovr.com/en/stable/)
    - [ ] [clang-tidy](https://clang.llvm.org/extra/clang-tidy/)

