#include <memory>
#include <cassert>
#include <queue>

#include "pathfinder/base.hpp"

#include "log.hpp"
#include "math.hpp"

namespace pathfinder {

PathFinderBase::PathFinderBase(const Map* map) : m_Map(map) {}

// LinearPathFinder also lives here, since it is too small to get it's
// own implementation file
Path LinearPathFinder::CalculatePath(WorldPos start, WorldPos end)
{
  auto path = Path{end};
  return path;
}

} // pathfinder namespace
