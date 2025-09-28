#include <queue>

#include "dijkstra.hpp"

#include "base.hpp"
#include "utils.hpp"
#include "math.hpp"
#include "map.hpp"

namespace pathfinder {

Path Dijkstra::CalculatePath(WorldPos start_world, WorldPos end_world)
{
  using QueueEntry = utils::QueueEntry;

  if (!m_Map) return {};

  const TilePos start = m_Map->WorldToTile(start_world);
  const TilePos end   = m_Map->WorldToTile(end_world);

  if (!m_Map->IsTilePosValid(start) || !m_Map->IsTilePosValid(end))
      return {};
  if (start == end) return {};

  // clear previous run
  m_CameFrom.clear();
  m_Cost.clear();

  std::priority_queue<QueueEntry, std::vector<QueueEntry>, std::greater<>> frontier;
  frontier.push({0.0f, start});
  m_CameFrom[start] = start;   // sentinel
  m_Cost[start]     = 0.0f;

  while (!frontier.empty())
  {
      const QueueEntry current = frontier.top();
      frontier.pop();

      if (current.tile == end)          // early exit
          break;

      for (TilePos next : m_Map->GetNeighbors(current.tile))
      {
          // cost of moving to neighbour (uniform 1.0 matches original BFS)
          const float newCost = m_Cost[current.tile] + m_Map->GetCost(next);

          if (!m_Cost.count(next) || newCost < m_Cost[next])
          {
              m_Cost[next]     = newCost;
              m_CameFrom[next] = current.tile;
              frontier.push({newCost, next});
          }
      }
  }

  // reconstruct path
  if (!m_CameFrom.count(end))
      return {};                   // goal never reached

  Path path;
  TilePos cur = end;
  path.push_back(m_Map->TileToWorld(cur));

  while (cur != start)
  {
      cur = m_CameFrom[cur];
      path.push_back(m_Map->TileToWorld(cur));
  }
  std::reverse(path.begin(), path.end());
  return path;
}

} // pathfinder namespace
