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

using Collision = std::pair<std::weak_ptr<Entity>, std::weak_ptr<Entity>>;

struct SelectionBox
{
  WindowPos start, end;
  WindowSize size;
  bool active;
};

class PathFindingDemo {
public:
  PathFindingDemo(int width, int height);
  ~PathFindingDemo();

  PathFindingDemo(const PathFindingDemo &m) = delete;
  PathFindingDemo(PathFindingDemo &&m) = delete;
  PathFindingDemo &operator=(const PathFindingDemo &) = delete;
  PathFindingDemo &operator=(PathFindingDemo &&) = delete;

  std::vector<std::shared_ptr<Entity>>& GetEntities() { return m_Entities; }
  const Map& GetMap() const { return m_Map; }
  const Camera& GetCamera() const { return m_Camera; }
  bool IsExitRequested() const { return m_ExitRequested; }

  void AddEntity(std::shared_ptr<Entity> e);
  void CreateMap();
  void UpdateWorld();
  void HandleActions(const std::vector<UserAction> &actions);
  WorldPos GetRandomPosition() const;

  void SelectEntitiesInRectangle(WorldPos A, WorldPos B);
  bool IsSelectionBoxActive() const { return m_SelectionBox.active; }
  std::pair<WindowPos, WindowSize> GetSelectionBoxPosSize();

private:
  const std::vector<Collision>& GetEntityCollisions();

  bool m_ExitRequested = false;
  Map m_Map;
  Camera m_Camera;
  std::vector<std::shared_ptr<Entity>> m_Entities;
  std::unique_ptr<pathfinder::PathFinderBase> m_PathFinder;
  std::vector<std::weak_ptr<Entity>> m_SelectedEntities;
  SelectionBox m_SelectionBox;
};
