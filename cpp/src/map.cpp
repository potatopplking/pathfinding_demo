#include <cassert>
#include <vector>

#include "log.hpp"
#include "map.hpp"
#include "tile.hpp"

Map::Map(int rows, int cols) : m_Cols(cols), m_Rows(rows) {
  LOG_DEBUG("cols = ", cols, " rows = ", rows);
  m_Tiles = std::vector<std::vector<const Tile *>>{};
  for (size_t row = 0; row < m_Rows; row++) {
    m_Tiles.push_back(std::vector<const Tile *>{});
    for (size_t col = 0; col < m_Cols; col++) {
        m_Tiles[row].push_back(&tile_types.at(TileType::GRASS));
    }
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
  if (p.x < 0 || p.y < 0)
    return false;
  size_t row = static_cast<size_t>(p.x);
  size_t col = static_cast<size_t>(p.y);

  return row < m_Tiles.size() && col < m_Tiles[0].size();
}


std::vector<TilePos> Map::GetNeighbors(TilePos center) const
{
  std::vector<TilePos> neighbours;
  neighbours.reserve(4);

  std::array<TilePos, 4> candidates = {
    center + TilePos{ 1,  0},
    center + TilePos{-1,  0},
    center + TilePos{ 0,  1},
    center + TilePos{ 0, -1},
  };

  for (const auto& c : candidates) {
    if (IsTilePosValid(c))
        neighbours.push_back(c);
  }
  return neighbours;
}


void Map::PaintCircle(TilePos center, unsigned radius, TileType tile_type)
{
  // get rectangle that wraps the circle
  TilePos corner1 = TilePos{center.x - radius, center.y - radius};
  TilePos corner2 = TilePos{center.x + radius, center.y + radius};
  // iterate through all valid points, setting the type
  const unsigned radius_squared = radius * radius; 
  for (int x = corner1.x; x < corner2.x; x++) {
    for (int y = corner1.y; y < corner2.y; y++) {
      TilePos current_tile = {x, y}; 
      unsigned distance_squared = center.distance_squared(current_tile);
      if (IsTilePosValid(current_tile) && distance_squared < radius_squared)
      {
        // y is row, x is col
        m_Tiles[y][x] = &tile_types.at(tile_type);
      }
    }
  }
}

void Map::PaintLine(TilePos start_tile, TilePos stop_tile, double width, TileType tile_type)
{
  const Vec2D<double> start(start_tile);
  const Vec2D<double> stop(stop_tile);
  const double line_length = start.distance(stop);
  const Vec2D<double> step((stop-start)/line_length);
  const Vec2D<double> ortho = step.orthogonal();
  LOG_DEBUG("step = ", step, " ortho = ", ortho);

  for (double t = 0; t < line_length; t += 1.0) {
    for (double ortho_t = 0; ortho_t < width; ortho_t += 0.1) {
      auto tile_pos = start + step * t + ortho * ortho_t;
      size_t row = static_cast<size_t>(tile_pos.x);
      size_t col = static_cast<size_t>(tile_pos.y);
      if (IsTilePosValid(tile_pos)) {
        m_Tiles[row][col] = &tile_types.at(tile_type);
      }
    }
  }
}

