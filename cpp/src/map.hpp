#pragma once

#include <vector>

#include "math.hpp"
#include "tile.hpp"

using TileGrid = std::vector<std::vector<const Tile *>>;

class Map {
public:
  static constexpr float TILE_SIZE = 10.0f; // tile size in world

  Map(int rows, int cols);
  Map() : Map(0, 0) {}

  Map(const Map&) = delete;
  Map(Map&&) = delete;
  Map& operator=(const Map&) = delete;
  Map& operator=(Map&&) = delete;

  const TileGrid &GetMapTiles() const { return m_Tiles; }

  // coordinate conversion functions
  WorldPos TileToWorld(TilePos p) const;
  WorldPos TileEdgeToWorld(TilePos p) const;
  TilePos WorldToTile(WorldPos p) const;

  WorldPos GetTileSize() const;
  const Tile *GetTileAt(TilePos p) const;
  const Tile *GetTileAt(WorldPos p) const;

  bool IsTilePosValid(TilePos p) const;

  // methods for drawing on the map
  void PaintCircle(TilePos center, unsigned radius, TileType tile_type);
  void PaintLine(TilePos start, TilePos stop, double width, TileType tile_type);
  void PaintRectangle(TilePos first_corner, TilePos second_corner, TileType tile_type);

  std::vector<TilePos> GetNeighbors(TilePos center) const;
  float GetCost(TilePos pos) const { return GetTileAt(pos)->cost; }

  template <typename T> double GetTileVelocityCoeff(T p) const {
    return 1.0 / GetTileAt(p)->cost;
  }

private:
  TileGrid m_Tiles;
  size_t m_Cols = 0;
  size_t m_Rows = 0;
};
