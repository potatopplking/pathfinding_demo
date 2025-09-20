all: test zomberman
# TODO add extra warnings
# TODO linter?

zomberman: src/main.cpp src/array.hpp
	g++ -Wall -ggdb3 -lSDL3 -lSDL3_image -std=c++23 -lGLEW -lGL -o zomberman src/main.cpp

test: src/test.cpp src/array.hpp
	g++ -Wall -Wextra -Wpedantic -ggdb3 -std=c++23 -lgtest -o test src/test.cpp
