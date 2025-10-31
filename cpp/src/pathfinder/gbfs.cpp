#include <queue>

#include "gbfs.hpp"

#include "base.hpp"
#include "map.hpp"
#include "math.hpp"
#include "pathfinder/utils.hpp"

namespace pathfinder {

float GBFS::Heuristic(const TilePos &a, const TilePos &b) {
  return static_cast<float>(std::abs(a.x() - b.x()) + std::abs(a.y() - b.y()));
}

Path GBFS::CalculatePath(WorldPos start_world, WorldPos end_world) {
  using QueueEntry = pathfinder::utils::QueueEntry;

  if (!m_Map)
    return {};

  const TilePos start = m_Map->WorldToTile(start_world);
  const TilePos end = m_Map->WorldToTile(end_world);

  if (!m_Map->IsTilePosValid(start) || !m_Map->IsTilePosValid(end))
    return {};
  if (start == end)
    return {};

  m_CameFrom.clear();

  std::priority_queue<QueueEntry, std::vector<QueueEntry>, std::greater<>>
      frontier;
  frontier.push({Heuristic(start, end), start});
  m_CameFrom[start] = start; // sentinel

  while (!frontier.empty()) {
    const QueueEntry current = frontier.top();
    frontier.pop();

    if (current.tile == end) // early exit
      break;

    for (TilePos next : m_Map->GetNeighbors(current.tile)) {
      if (!m_CameFrom.count(next)) // not visited
      {
        m_CameFrom[next] = current.tile;
        frontier.push({Heuristic(end, next), next});
      }
    }
  }

  // reconstruct path
  if (!m_CameFrom.count(end))
    return {}; // goal never reached

  Path path;
  TilePos cur = end;
  path.push_back(m_Map->TileToWorld(cur));

  while (cur != start) {
    cur = m_CameFrom[cur];
    path.push_back(m_Map->TileToWorld(cur));
  }
  std::reverse(path.begin(), path.end());
  return path;
}

} // namespace pathfinder
