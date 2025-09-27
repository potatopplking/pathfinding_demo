#pragma once

#include <map>
#include <string_view>
#include <cstdint>

struct Tile {
  float cost;
  uint8_t R, G, B, A;
};

static const std::map<std::string_view, Tile> tile_types = {
    {"Grass", {1.0, 0, 200, 0, 255}},
    {"Mud", {2.0, 100, 100, 100, 255}},
    {"Road", {0.5, 200, 200, 200, 255}},
    {"Water", {10.0, 0, 50, 200, 255}},
};

