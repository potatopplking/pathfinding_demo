all: test pathfinding
# TODO add extra warnings
# TODO linter?

pathfinding: 
	g++ -Wall -ggdb3 -lSDL3 -lSDL3_image -std=c++23 -lGLEW -lGL -o pathfinding src/main.cpp src/sprite.cpp src/entities.cpp src/gameloop.cpp src/map.cpp src/pathfindingdemo.cpp src/tile.cpp

test: src/test.cpp src/array.hpp
	g++ -Wall -Wextra -Wpedantic -ggdb3 -std=c++23 -lgtest -o test src/test.cpp
