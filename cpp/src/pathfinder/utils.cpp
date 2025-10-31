#include <memory>

#include "utils.hpp"

#include "base.hpp"
#include "log.hpp"
#include "map.hpp"
#include "math.hpp"
#include "pathfinder/bfs.hpp"
#include "pathfinder/dijkstra.hpp"
#include "pathfinder/gbfs.hpp"

namespace pathfinder {
namespace utils {

std::unique_ptr<PathFinderBase> create(PathFinderType type, const Map *map) {
  using namespace pathfinder;
  switch (type) {
  case PathFinderType::LINEAR:
    return std::make_unique<LinearPathFinder>(map);
  case PathFinderType::BFS:
    return std::make_unique<BFS>(map);
  case PathFinderType::DIJKSTRA:
    return std::make_unique<Dijkstra>(map);
  case PathFinderType::GBFS:
    return std::make_unique<GBFS>(map);
  case PathFinderType::COUNT:
    LOG_WARNING("Incorrect pathfinder type");
    return nullptr;
  };
  return nullptr;
}

} // namespace utils
} // namespace pathfinder
