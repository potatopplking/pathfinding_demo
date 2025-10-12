cmake_minimum_required(VERSION 3.20)
project(PathfindingDemo)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL3 REQUIRED sdl3)
pkg_check_modules(SDL3_image REQUIRED sdl3-image)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

# Add Google Test
find_package(GTest REQUIRED)

# Include directories
include_directories(cpp/src)

# Source files for the main executable
set(MAIN_SOURCES
    cpp/src/main.cpp
    cpp/src/camera.cpp
    cpp/src/entities.cpp
    cpp/src/gameloop.cpp
    cpp/src/map.cpp
    cpp/src/pathfinder/base.cpp
    cpp/src/pathfinder/bfs.cpp
    cpp/src/pathfinder/dijkstra.cpp
    cpp/src/pathfinder/gbfs.cpp
    cpp/src/pathfinder/utils.cpp
    cpp/src/pathfindingdemo.cpp
    cpp/src/sprite.cpp
    cpp/src/tile.cpp
    cpp/src/user_input.cpp
    cpp/src/window.cpp
)

# Header files (for IDE support)
set(HEADERS
    cpp/src/array.hpp
    cpp/src/camera.hpp
    cpp/src/entities.hpp
    cpp/src/gameloop.hpp
    cpp/src/log.hpp
    cpp/src/map.hpp
    cpp/src/math.hpp
    cpp/src/pathfinder/base.hpp
    cpp/src/pathfinder/bfs.hpp
    cpp/src/pathfinder/dijkstra.hpp
    cpp/src/pathfinder/gbfs.hpp
    cpp/src/pathfinder/utils.hpp
    cpp/src/pathfindingdemo.hpp
    cpp/src/sprite.hpp
    cpp/src/tile.hpp
    cpp/src/user_input.hpp
    cpp/src/window.hpp
)

# Create main executable
add_executable(pathfinding_demo ${MAIN_SOURCES} ${HEADERS})

# Link libraries for main executable
target_link_libraries(pathfinding_demo 
    ${SDL3_LIBRARIES} 
    ${SDL3_image_LIBRARIES}
    OpenGL::GL
    GLEW::GLEW
)

# Add compile flags
target_compile_options(pathfinding_demo PRIVATE ${SDL3_CFLAGS_OTHER})

# Test executable
add_executable(tests cpp/test/test.cpp)
target_link_libraries(tests GTest::gtest GTest::gtest_main)

# Enable testing
enable_testing()
add_test(NAME unit_tests COMMAND tests)

# Compiler-specific options
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(pathfinding_demo PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(tests PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Debug/Release configurations
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
