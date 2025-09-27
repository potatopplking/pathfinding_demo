#pragma once

#include <memory>
#include <optional>
#include <queue>
#include <vector>

#include "entities.hpp"
#include "map.hpp"
#include "user_input.hpp"
#include "log.hpp"

class PathFindingDemo {
public:
  PathFindingDemo(int width, int height)
      : m_Width(width), m_Height(height), // TODO delete width, height
        m_Map(width, height) {
    LOG_DEBUG(".");
  }

  ~PathFindingDemo() { LOG_DEBUG("."); }

  PathFindingDemo(const PathFindingDemo &m) = delete;
  PathFindingDemo(PathFindingDemo &&m) = delete;

  void AddEntity(std::shared_ptr<Entity> e) {
    // TODO emplace_back
    m_Entities.push_back(e);
  }

  void CreateMap() {
    m_Entities.clear();
    m_Player = std::make_shared<Player>();
    m_Player->SetPosition(Vec2D<float>{200.0f, 200.0f});
    m_Entities.push_back(m_Player);
  }

  std::shared_ptr<Player> GetPlayer() { return m_Player; }

  Vec2D<float> GetRandomPosition() const { return Vec2D<float>{0.0, 0.0}; }

  std::vector<std::shared_ptr<Entity>> &GetEntities() { return m_Entities; }

  std::optional<WorldPos> GetMoveTarget() {
    WorldPos current_player_pos = GetPlayer()->GetPosition();

    if (m_MoveQueue.empty()) {
      return {};
    }

    WorldPos next_player_pos = m_MoveQueue.front();

    if (current_player_pos.distance(next_player_pos) > 10.0) {
      // target not reached yet
      return next_player_pos; 
    }
    // target reached, pop it
    m_MoveQueue.pop();
    // return nothing - we'll get the next value in the next iteration
    return {};
  }


  void UpdatePlayerVelocity()
  {
    auto player = GetPlayer();
    auto current_pos = player->GetPosition();
    double tile_velocity_coeff = m_Map.GetTileVelocityCoeff(current_pos);
    auto next_pos = GetMoveTarget();
    auto velocity = WorldPos{};
    if (next_pos) {
      velocity = next_pos.value() - current_pos;
      velocity.normalize();
      LOG_DEBUG("I want to move to: ", next_pos.value(), ", velocity: ", velocity);
    }
    player->SetActualVelocity(velocity * tile_velocity_coeff);
    float time_delta = 1.0f;
    player->Update(time_delta);
  }

  void HandleActions(const std::vector<UserAction> &actions) {
    for (const auto &action : actions) {
      if (action.type == UserAction::Type::EXIT) {
        LOG_INFO("Exit requested");
        m_ExitRequested = true;
      } else if (action.type == UserAction::Type::FIRE) {
        LOG_INFO("Fire");
        // AddEntity(m_Player->CreateBomb());
      } else if (action.type == UserAction::Type::MOVE) {
        LOG_INFO("Move direction ", action.Argument.position);
        m_Player->SetRequestedVelocity(action.Argument.position * 4.0f);
      } else if (action.type == UserAction::Type::MOVE_TARGET) {
        WorldPos wp = action.Argument.position;
        TilePos p = m_Map.WorldToTile(wp);
        LOG_INFO("Clearing current move queue and inserting new target: ", wp);
        std::queue<WorldPos> empty;
        std::swap(empty, m_MoveQueue);
        m_MoveQueue.push(wp);
      }
    };
  }

  const Map &GetMap() const { return m_Map; }

  bool IsExitRequested() const { return m_ExitRequested; }

private:
  int m_Width;
  int m_Height;
  bool m_ExitRequested = false;
  std::vector<std::shared_ptr<Entity>> m_Entities;
  std::shared_ptr<Player> m_Player;
  std::queue<WorldPos> m_MoveQueue;
  Map m_Map;
};
