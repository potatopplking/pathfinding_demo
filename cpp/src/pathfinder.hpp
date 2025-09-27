#pragma once

#include <vector>
#include <memory>

#include "math.hpp"
#include "map.hpp"

namespace pathfinder {

using Path = std::vector<WorldPos>;

enum class PathFinderType {
  LINEAR = 1,
  BFS,
  COUNT,
};

class PathFinderBase {
public:
  PathFinderBase() = default;
  ~PathFinderBase() = default;

  PathFinderBase(const PathFinderBase&) = delete;
  PathFinderBase(PathFinderBase&&) = delete;
  PathFinderBase& operator=(const PathFinderBase&) = delete;
  PathFinderBase& operator=(PathFinderBase&&) = delete;

  void SetMap(std::shared_ptr<Map> map);
  virtual Path CalculatePath(WorldPos target) = 0;

private:
  std::shared_ptr<Map> m_Map;
};


class LinearPathFinder : public PathFinderBase {
  Path CalculatePath(WorldPos target) override;
};

std::unique_ptr<PathFinderBase> create(PathFinderType type);


} // pathfinder namespace
  
