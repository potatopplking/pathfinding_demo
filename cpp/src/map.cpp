#include <cassert>
#include <vector>

#include "log.hpp"
#include "map.hpp"
#include "tile.hpp"

Map::Map(int rows, int cols) : m_Cols(cols), m_Rows(rows) {
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

WorldPos Map::TileToWorld(TilePos p) const {
  return WorldPos{(p.x + 0.5) * TILE_SIZE, (p.y + 0.5) * TILE_SIZE}; 
}

WorldPos Map::TileEdgeToWorld(TilePos p) const {
  return WorldPos{p.x * TILE_SIZE, p.y * TILE_SIZE}; 
}

TilePos Map::WorldToTile(WorldPos p) const {
  return TilePos{p.x / TILE_SIZE, p.y / TILE_SIZE};
}

WorldPos Map::GetTileSize() const { return WorldPos{TILE_SIZE, TILE_SIZE}; }

const Tile *Map::GetTileAt(TilePos p) const {
  assert(IsTilePosValid(p));
  size_t row = p.x;
  size_t col = p.y;

  return m_Tiles[row][col];
}

const Tile *Map::GetTileAt(WorldPos p) const {
  return GetTileAt(WorldToTile(p));
}

bool Map::IsTilePosValid(TilePos p) const {
  size_t row = p.x;
  size_t col = p.y;

  return row < m_Tiles.size() && col < m_Tiles[0].size();
}


std::vector<TilePos> Map::GetNeighbors(TilePos center) const
{
  std::vector<TilePos> neighbours;
  neighbours.reserve(4);

  std::array<TilePos, 4> candidates = {
    center + TilePos{1,0},
    center + TilePos{-1,0},
    center + TilePos{0, 1},
    center + TilePos{0, -1},
  };

  for (const auto& c : candidates) {
    if (IsTilePosValid(c))
        neighbours.push_back(c);
  }
  return neighbours;
}
//  std::vector<TilePos> neighbours;
//  neighbours.reserve(8);
//  for (int dx = -1; dx <= 1; ++dx) {
//      for (int dy = -1; dy <= 1; ++dy) {
//          if (dx == 0 && dy == 0) continue;
//          TilePos p{center.x + dx, center.y + dy};
//          if (IsTilePosValid(p)) neighbours.push_back(std::move(p));
//      }
//  }
//  return neighbours;
