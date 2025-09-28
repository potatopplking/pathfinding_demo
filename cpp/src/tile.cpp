#include <array>
#include <unordered_map>

#include "tile.hpp"

// we could use array here, but this is more explicit,
// and we don't access tile_types that often, so it should be ok
const std::unordered_map<TileType, Tile> tile_types = {
    { TileType::GRASS, Tile{1.0, 0, 200, 0, 255}},
    { TileType::MUD,   Tile{2.0, 100, 100, 100, 255}},
    { TileType::ROAD,  Tile{0.5, 20, 20, 20, 255}},
    { TileType::WATER, Tile{10.0, 0, 50, 200, 255}},
};
