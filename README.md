# Pathfinding demo

This is a demo of pathfinding on a 2D grid. It consists of 2 main parts:

* python notes and implementation
    * jupyter notebook file
    * standalone python script
* C++ demo - **work in progress**

## Python

### Standalone script

Run `python pathfinding_demo.py`. Requires numpy and matplotlib to be installed.

### Jupyter notebook

Contains the same demo as the standalone script and some notes. To run, either run your own Jupyter Lab, or use the [official website](https://jupyter.org/try-jupyter/lab/):

* click the icon "Upload files"
* select pathfinding_demo.ipynb
* file should be now visible in the left bar. Double-click it and confirm default kernel selection
* run all the cells one by one (play button on the top) or run all of at once using menu "Run -> Run All Cells"

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
    - [ ] add tiles (with cost) to map
        - [ ] conversion functions from tile coords to world coords
        - [ ] drawing tiles
        - [ ] add "terrain tiles" with different costs
    - [ ] add mouse-click movement (direct, no pathfinding, player always selected)
    - [ ] implement pathfinding
    - [ ] windows build?
