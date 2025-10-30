#include <queue>

#include "bfs.hpp"

#include "base.hpp"
#include "map.hpp"
#include "math.hpp"

namespace pathfinder {

Path BFS::CalculatePath(WorldPos start_world, WorldPos end_world) {
  if (m_Map == nullptr)
    return {};

  const TilePos start = m_Map->WorldToTile(start_world);
  const TilePos end = m_Map->WorldToTile(end_world);

  if (!m_Map->IsTilePosValid(start) || !m_Map->IsTilePosValid(end))
    return {};
  if (start == end) {
    return {};
  }
  // clear previous run
  m_CameFrom.clear();
  m_Distance.clear();

  std::queue<TilePos> frontier;
  frontier.push(start);
  m_CameFrom[start] = start;
  m_Distance[start] = 0.0f;

  // ---------------- build flow-field ----------------
  bool early_exit = false;
  while (!frontier.empty() && !early_exit) {
    TilePos current = frontier.front();
    frontier.pop();

    for (TilePos next : m_Map->GetNeighbors(current)) {
      if (m_CameFrom.find(next) == m_CameFrom.end()) { // not visited
        frontier.push(next);
        m_Distance[next] = m_Distance[current] + 1.0f;
        m_CameFrom[next] = current;

        if (next == end) { // early exit
          early_exit = true;
          break;
        }
      }
    }
  }

  // --------------- reconstruct path -----------------
  if (m_CameFrom.find(end) == m_CameFrom.end())
    return {}; // end not reached

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
