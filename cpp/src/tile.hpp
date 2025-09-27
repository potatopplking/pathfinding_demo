#pragma once

#include <cstdint>
#include <map>
#include <string_view>

struct Tile {
  float cost;
  uint8_t R, G, B, A;
};

extern const std::map<std::string_view, Tile> tile_types;
