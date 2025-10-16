cmake_minimum_required(VERSION 3.20)

# Vcpkg integration for Windows
if(WIN32)
    # Try to find vcpkg automatically
    if(DEFINED ENV{VCPKG_ROOT})
        set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
            CACHE STRING "Vcpkg toolchain file")
        message(STATUS "Using vcpkg from: $ENV{VCPKG_ROOT}")
    elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")
        set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"
            CACHE STRING "Vcpkg toolchain file")
        message(STATUS "Using local vcpkg installation")
    else()
        message(WARNING "vcpkg not found. Please set VCPKG_ROOT environment variable or install vcpkg locally.")
        message(STATUS "To install dependencies with vcpkg:")
        message(STATUS "  vcpkg install sdl3 sdl3-image glew opengl gtest")
    endif()
endif()

project(PathfindingDemo)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Platform-specific package finding
if(WIN32)
    # Windows: Use vcpkg-compatible find_package with CONFIG
    find_package(SDL3 CONFIG REQUIRED)
    find_package(SDL3_image CONFIG REQUIRED)
    find_package(OpenGL REQUIRED)
    find_package(GLEW REQUIRED)
    find_package(GTest CONFIG REQUIRED)
    
    # Set vcpkg target names for consistent linking
    set(SDL3_TARGET SDL3::SDL3)
    set(SDL3_IMAGE_TARGET $<IF:$<TARGET_EXISTS:SDL3_image::SDL3_image-shared>,SDL3_image::SDL3_image-shared,SDL3_image::SDL3_image-static>)
    set(GLEW_TARGET GLEW::GLEW)
else()
    # Linux/Unix: Use pkg-config
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL3 REQUIRED sdl3)
    pkg_check_modules(SDL3_image REQUIRED sdl3-image)
    find_package(OpenGL REQUIRED)
    find_package(GLEW REQUIRED)
    find_package(GTest REQUIRED)
    
    # Set target names for Unix systems
    set(SDL3_TARGET ${SDL3_LIBRARIES})
    set(SDL3_IMAGE_TARGET ${SDL3_image_LIBRARIES})
    set(GLEW_TARGET GLEW::GLEW)
endif()

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

# Platform-specific linking
if(WIN32)
    # Windows: Use vcpkg targets
    target_link_libraries(pathfinding_demo 
        ${SDL3_TARGET}
        ${SDL3_IMAGE_TARGET}
        OpenGL::GL
        ${GLEW_TARGET}
    )

    # Add compile flags for Windows (vcpkg handles most of this automatically)
    if(MSVC)
        if(SDL3_CFLAGS_OTHER)
            target_compile_options(pathfinding_demo PRIVATE ${SDL3_CFLAGS_OTHER})
        endif()
    endif()
else()
    # Unix/Linux: Use pkg-config variables
    target_link_libraries(pathfinding_demo 
        ${SDL3_TARGET}
        ${SDL3_IMAGE_TARGET}
        OpenGL::GL
        ${GLEW_TARGET}
    )
    
    # Add compile flags for Unix/Linux
    target_compile_options(pathfinding_demo PRIVATE ${SDL3_CFLAGS_OTHER})
endif()


# Copy resources after build
add_custom_command(TARGET pathfinding_demo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/cpp/resources
    $<TARGET_FILE_DIR:pathfinding_demo>/resources
)

# Unit tests executable
add_executable(unit_tests 
    cpp/test/test.cpp
)
if(WIN32)
    target_link_libraries(unit_tests GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
else()
    target_link_libraries(unit_tests GTest::gtest GTest::gtest_main)
endif()

# Performance tests executable
add_executable(performance_tests 
    cpp/test/collision_performance.cpp
)
if(WIN32)
    target_link_libraries(performance_tests GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
else()
    target_link_libraries(performance_tests GTest::gtest GTest::gtest_main)
endif()

# Enable testing
enable_testing()
add_test(NAME unit_tests COMMAND unit_tests)
add_test(NAME performance_tests COMMAND performance_tests)

# Compiler-specific options with MSVC support
if(MSVC)
    # MSVC-specific flags: disable permissive mode, enable high warning level
    target_compile_options(pathfinding_demo PRIVATE /W4 /permissive-)
    target_compile_options(unit_tests PRIVATE /W4 /permissive-)
    target_compile_options(performance_tests PRIVATE /W4 /permissive-)
    
    # Additional MSVC flags for C++23 and modern standards
    target_compile_options(pathfinding_demo PRIVATE /Zc:__cplusplus /Zc:preprocessor)
    target_compile_options(unit_tests PRIVATE /Zc:__cplusplus /Zc:preprocessor)
    target_compile_options(performance_tests PRIVATE /Zc:__cplusplus /Zc:preprocessor)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # GCC/Clang flags
    target_compile_options(pathfinding_demo PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(unit_tests PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(performance_tests PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Platform-specific build configurations
if(MSVC)
    # MSVC flags
    set(CMAKE_CXX_FLAGS_DEBUG "/Od /Zi /RTC1")
    set(CMAKE_CXX_FLAGS_RELEASE "/O2 /DNDEBUG")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/O2 /Zi /DNDEBUG")
    set(CMAKE_CXX_FLAGS_MINSIZEREL "/O1 /DNDEBUG")
else()
    # GCC/Clang flags
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
endif()
