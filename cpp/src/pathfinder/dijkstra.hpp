#pragma once

#include <string_view>
#include <unordered_map>

#include "base.hpp"

#include "map.hpp"
#include "math.hpp"

namespace pathfinder {

class Dijkstra final : public PathFinderBase {

public:
  Dijkstra(const Map *m) : PathFinderBase(m) {}
  Path CalculatePath(WorldPos start, WorldPos end) override;
  const std::string_view &GetName() const override { return m_Name; }

private:
  const std::string_view m_Name = "Dijkstra's Algorithm";
  std::unordered_map<TilePos, double, TilePosHash> m_Cost;
  std::unordered_map<TilePos, TilePos, TilePosHash> m_CameFrom;
};

} // namespace pathfinder
