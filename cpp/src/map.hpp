#pragma once

#include <vector>
#include <cassert>

#include "math.hpp"
#include "tile.hpp"

class Map {

  // TODO using = ... for tile vector

public:
  static constexpr float TILE_SIZE = 100.0f; // tile size in world

  Map(int rows, int cols) : m_Cols(cols), m_Rows(rows) {
    bool sw = true;
    LOG_DEBUG("cols = ", cols, " rows = ", rows);
    m_Tiles = std::vector<std::vector<const Tile *>>{};
    for (size_t row = 0; row < m_Rows; row++) {
      m_Tiles.push_back(std::vector<const Tile *>{});
      for (size_t col = 0; col < m_Cols; col++) {
        if (sw)
          m_Tiles[row].push_back(&tile_types.at("Grass"));
        else
          m_Tiles[row].push_back(&tile_types.at("Road"));
        sw = !sw;
      }
      sw = !sw;
    }
  }
  Map() : Map(0, 0) {}

  const std::vector<std::vector<const Tile *>> &GetMapTiles() const {
    return m_Tiles;
  }

  WorldPos TileToWorld(TilePos p) const {
    return WorldPos{p.x * TILE_SIZE, p.y * TILE_SIZE};
  }

  TilePos WorldToTile(WorldPos p) const {
    return TilePos{p.x / TILE_SIZE, p.y / TILE_SIZE};
  }

  Vec2D<float> GetTileSize() const {
    return Vec2D<float>{TILE_SIZE, TILE_SIZE};
  }

  const Tile* GetTileAt(TilePos p) const {
    assert(IsTilePosValid(p));
    size_t row = p.x;
    size_t col = p.y;

    return m_Tiles[row][col];

  }

  const Tile* GetTileAt(WorldPos p) const {
    return GetTileAt(WorldToTile(p));
  }

  bool IsTilePosValid(TilePos p) const {
    size_t row = p.x;
    size_t col = p.y;

    return row < m_Tiles.size() && col < m_Tiles[0].size();
  }

  template<typename T>
  double GetTileVelocityCoeff(T p) const {
    return 1.0 / GetTileAt(p)->cost;
  }

private:
  // std::vector<std::vector<const Tile*>> m_Tiles;
  std::vector<std::vector<const Tile *>> m_Tiles;
  size_t m_Cols = 0;
  size_t m_Rows = 0;
};
