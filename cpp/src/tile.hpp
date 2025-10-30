#pragma once

#include <array>
#include <cstdint>
#include <map>
#include <string_view>
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
