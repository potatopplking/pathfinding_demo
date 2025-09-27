#include <memory>
#include <optional>
#include <queue>
#include <vector>

#include "pathfindingdemo.hpp"

#include "entities.hpp"
#include "log.hpp"
#include "map.hpp"
#include "user_input.hpp"
#include "pathfinder.hpp"

PathFindingDemo::PathFindingDemo(int width, int height) :
  m_Map(width, height)
{
  LOG_DEBUG(".");
  // set default pathfinder method
  m_PathFinder = pathfinder::create(pathfinder::PathFinderType::LINEAR, (const Map*)&m_Map);
}

PathFindingDemo::~PathFindingDemo() { LOG_DEBUG("."); }

void PathFindingDemo::AddEntity(std::shared_ptr<Entity> e) {
  // TODO emplace_back
  m_Entities.push_back(e);
}

void PathFindingDemo::CreateMap() {
  m_Entities.clear();
  m_Player = std::make_shared<Player>();
  m_Player->SetPosition(WorldPos{200.0f, 200.0f});
  m_Entities.push_back(m_Player);
}

WorldPos PathFindingDemo::GetRandomPosition() const {
  return WorldPos{0.0, 0.0};
}

std::optional<WorldPos> PathFindingDemo::GetMoveTarget() {
  WorldPos current_player_pos = GetPlayer()->GetPosition();

  if (m_Path.empty()) {
    return {};
  }

  WorldPos next_player_pos = m_Path.front();

  if (current_player_pos.distance(next_player_pos) > 10.0) {
    // target not reached yet
    return next_player_pos;
  }
  // target reached, pop it
  //m_MoveQueue.pop();
  m_Path.erase(m_Path.begin());
  // return nothing - if there's next point in the queue,
  // we'll get it in the next iteration
  return {};
}

void PathFindingDemo::UpdatePlayerVelocity() {
  auto player = GetPlayer();
  auto current_pos = player->GetPosition();
  double tile_velocity_coeff = m_Map.GetTileVelocityCoeff(current_pos);
  auto next_pos = GetMoveTarget();
  WorldPos velocity = WorldPos{};
  if (next_pos) {
    velocity = next_pos.value() - current_pos;
    velocity.normalize();
    LOG_DEBUG("I want to move to: ", next_pos.value(),
              ", velocity: ", velocity);
  }
  player->SetActualVelocity(velocity * tile_velocity_coeff);
  float time_delta = 1.0f;
  player->Update(time_delta);
}

void PathFindingDemo::HandleActions(const std::vector<UserAction> &actions) {
  for (const auto &action : actions) {
    if (action.type == UserAction::Type::EXIT) {
      LOG_INFO("Exit requested");
      m_ExitRequested = true;
    } else if (action.type == UserAction::Type::SET_MOVE_TARGET) {
      WorldPos wp = action.Argument.position;
      LOG_INFO("Calculating path to target: ", wp);
      m_Path = m_PathFinder->CalculatePath(m_Player->GetPosition(), wp);
      LOG_INFO("Done, path node count: ", m_Path.size());
    } else if (action.type == UserAction::Type::SELECT_PATHFINDER) {
      using namespace pathfinder;
      PathFinderType type = static_cast<PathFinderType>(action.Argument.number);
      m_PathFinder = pathfinder::create(type, (const Map*)&m_Map);
      LOG_INFO("Switched to path finding method: ", m_PathFinder->GetName());
    }
  };
}
