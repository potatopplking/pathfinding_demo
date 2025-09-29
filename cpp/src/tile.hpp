#pragma once

#include <cstdint>
#include <map>
#include <string_view>
#include <array>
#include <unordered_map>

struct Tile {
  float cost;
  uint8_t R, G, B, A;
};

enum class TileType {
  GRASS,
  WOOD,
  ROAD,
  WATER,
  WALL,
};

extern const std::unordered_map<TileType, Tile> tile_types;
