#pragma once

#include <memory>

#include "pathfinder/base.hpp"

#include "map.hpp"
#include "math.hpp"

namespace pathfinder {
namespace utils {

struct QueueEntry {
  float cost;
  TilePos tile;

  // min-heap -> smallest cost on top
  bool operator>(const QueueEntry &o) const noexcept { return cost > o.cost; }
};

std::unique_ptr<pathfinder::PathFinderBase>
create(pathfinder::PathFinderType type, const Map *map);

} // namespace utils
} // namespace pathfinder
