#pragma once

#include <vector>
#include <memory>
#include <unordered_map>

#include "math.hpp"
#include "map.hpp"

namespace pathfinder {

using Path = std::vector<WorldPos>;

enum class PathFinderType {
  LINEAR = 1,
  BFS,
  DIJKSTRA,
  COUNT,
};

class PathFinderBase {
public:
  PathFinderBase(const Map* m);
  ~PathFinderBase() = default;

  PathFinderBase(const PathFinderBase&) = delete;
  PathFinderBase(PathFinderBase&&) = delete;
  PathFinderBase& operator=(const PathFinderBase&) = delete;
  PathFinderBase& operator=(PathFinderBase&&) = delete;

  virtual const std::string_view& GetName() const = 0; 
  virtual Path CalculatePath(WorldPos start, WorldPos end) = 0;

protected:
  const Map* m_Map;
};


class LinearPathFinder : public PathFinderBase {

public:
  LinearPathFinder(const Map* m): PathFinderBase(m) {}
  Path CalculatePath(WorldPos start, WorldPos end) override;
  const std::string_view& GetName() const override { return m_Name; }

private:
  const std::string_view m_Name = "Linear Path";
};


class BFS: public PathFinderBase {

public:
  BFS(const Map* m): PathFinderBase(m) {}
  Path CalculatePath(WorldPos start, WorldPos end) override;
  const std::string_view& GetName() const override { return m_Name; }

private:
  const std::string_view m_Name = "Breadth First Search";
  std::unordered_map<TilePos, double, TilePosHash> m_Distance;
  std::unordered_map<TilePos, TilePos, TilePosHash> m_CameFrom;
};

class Dijkstra: public PathFinderBase {

public:
  Dijkstra(const Map* m): PathFinderBase(m) {}
  Path CalculatePath(WorldPos start, WorldPos end) override;
  const std::string_view& GetName() const override { return m_Name; }

private:
  const std::string_view m_Name = "Dijkstra's Algorithm";
  std::unordered_map<TilePos, double, TilePosHash> m_Cost;
  std::unordered_map<TilePos, TilePos, TilePosHash> m_CameFrom;
};


std::unique_ptr<PathFinderBase> create(PathFinderType type, const Map* map);

} // pathfinder namespace
  
