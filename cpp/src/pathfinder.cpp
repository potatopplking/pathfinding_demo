#include <memory>
#include <cassert>

#include "pathfinder.hpp"
#include "log.hpp"

namespace pathfinder {

void PathFinderBase::SetMap(std::shared_ptr<Map> map)
{
  m_Map = map;
}

Path LinearPathFinder::CalculatePath(WorldPos target)
{
  auto path = Path{target};
  return path;
}

std::unique_ptr<PathFinderBase> create(PathFinderType type) {
  switch (type) {
    case PathFinderType::LINEAR:
      return std::move(std::make_unique<LinearPathFinder>());
    case PathFinderType::BFS:
    case PathFinderType::COUNT:
      LOG_WARNING("Incorrect pathfinder type");
      return nullptr;
  };
  return nullptr;
}

} // pathfinder namespace
