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

