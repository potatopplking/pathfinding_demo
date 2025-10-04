#pragma once

#include <memory>
#include <optional>
#include <queue>
#include <vector>

#include "entities.hpp"
#include "log.hpp"
#include "map.hpp"
#include "user_input.hpp"
#include "pathfinder/base.hpp"
#include "camera.hpp"

class PathFindingDemo {
public:
  PathFindingDemo(int width, int height);
  ~PathFindingDemo();

  PathFindingDemo(const PathFindingDemo &m) = delete;
  PathFindingDemo(PathFindingDemo &&m) = delete;
  PathFindingDemo &operator=(const PathFindingDemo &) = delete;
  PathFindingDemo &operator=(PathFindingDemo &&) = delete;

  std::shared_ptr<Player> GetPlayer() { return m_Player; }
  std::vector<std::shared_ptr<Entity>>& GetEntities() { return m_Entities; }
  const Map& GetMap() const { return m_Map; }
  const Camera& GetCamera() const { return m_Camera; }
  const pathfinder::Path& GetPath() const { return m_Path; }
  bool IsExitRequested() const { return m_ExitRequested; }

  void AddEntity(std::shared_ptr<Entity> e);
  void CreateMap();
  std::optional<WorldPos> GetMoveTarget();
  void UpdatePlayerVelocity();
  void HandleActions(const std::vector<UserAction> &actions);
  WorldPos GetRandomPosition() const;

private:
  bool m_ExitRequested = false;
  Map m_Map;
  Camera m_Camera;
  std::vector<std::shared_ptr<Entity>> m_Entities;
  std::shared_ptr<Player> m_Player;
  pathfinder::Path m_Path;
  std::unique_ptr<pathfinder::PathFinderBase> m_PathFinder;
};
