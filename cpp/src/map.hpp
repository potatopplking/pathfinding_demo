#pragma once

#include <vector>

#include "math.hpp"
#include "tile.hpp"

using TileGrid = std::vector<std::vector<const Tile *>>;

class Map {
public:
  static constexpr float TILE_SIZE = 100.0f; // tile size in world

  Map(int rows, int cols);
  Map() : Map(0, 0) {}

  Map(const Map&) = delete;
  Map(Map&&) = delete;
  Map& operator=(const Map&) = delete;
  Map& operator=(Map&&) = delete;

  const TileGrid &GetMapTiles() const { return m_Tiles; }

  WorldPos TileToWorld(TilePos p) const;
  TilePos WorldToTile(WorldPos p) const;

  WorldPos GetTileSize() const;
  const Tile *GetTileAt(TilePos p) const;
  const Tile *GetTileAt(WorldPos p) const;

  bool IsTilePosValid(TilePos p) const;

  template <typename T> double GetTileVelocityCoeff(T p) const {
    return 1.0 / GetTileAt(p)->cost;
  }

private:
  TileGrid m_Tiles;
  size_t m_Cols = 0;
  size_t m_Rows = 0;
};
