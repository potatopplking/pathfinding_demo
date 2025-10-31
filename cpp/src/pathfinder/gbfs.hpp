#pragma once

#include <string_view>
#include <unordered_map>

#include "base.hpp"

#include "map.hpp"
#include "math.hpp"

namespace pathfinder {

class GBFS final : public PathFinderBase {

public:
  GBFS(const Map *m) : PathFinderBase(m) {}
  Path CalculatePath(WorldPos start, WorldPos end) override;
  const std::string_view &GetName() const override { return m_Name; }

private:
  static float Heuristic(const TilePos &a, const TilePos &b);
  const std::string_view m_Name = "Greedy Best First Search";
  std::unordered_map<TilePos, TilePos, TilePosHash> m_CameFrom;
};

} // namespace pathfinder
