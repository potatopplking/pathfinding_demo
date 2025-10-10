#include <memory>
#include <optional>
#include <queue>
#include <vector>

#include "pathfindingdemo.hpp"

#include "entities.hpp"
#include "log.hpp"
#include "map.hpp"
#include "user_input.hpp"
#include "pathfinder/base.hpp"
#include "pathfinder/utils.hpp"
#include "tile.hpp"

PathFindingDemo::PathFindingDemo(int width, int height) :
  m_Map(width, height)
{
  LOG_DEBUG(".");
  // set default pathfinder method
  m_PathFinder = pathfinder::utils::create(pathfinder::PathFinderType::DIJKSTRA, (const Map*)&m_Map);
}

PathFindingDemo::~PathFindingDemo() { LOG_DEBUG("."); }

void PathFindingDemo::AddEntity(std::shared_ptr<Entity> e) {
  // TODO emplace_back
  m_Entities.push_back(e);
}

void PathFindingDemo::CreateMap() {
  // lake
  m_Map.PaintCircle(TilePos{50, 50}, 10, TileType::WATER);
  m_Map.PaintCircle(TilePos{75, 100}, 50, TileType::WATER);
  // river
  m_Map.PaintLine(TilePos{0,0}, TilePos{100,100}, 3.0, TileType::WATER);
  // road
  m_Map.PaintLine(TilePos{17,6}, TilePos{100,6}, 5.0, TileType::ROAD);
  m_Map.PaintLine(TilePos{10,17}, TilePos{10,100}, 5.0, TileType::ROAD);
  m_Map.PaintLine(TilePos{20,10}, TilePos{10,20}, 5.0, TileType::ROAD);
  // bridges
  m_Map.PaintLine(TilePos{50,75}, TilePos{70,75}, 5.0, TileType::WOOD);
  m_Map.PaintLine(TilePos{95,26}, TilePos{95,60}, 5.0, TileType::WOOD);
  // island
  m_Map.PaintRectangle(TilePos{70, 60}, TilePos{100,100}, TileType::GRASS);
  // walls
  m_Map.PaintLine(TilePos{71,60}, TilePos{90,60}, 1.0, TileType::WALL);
  m_Map.PaintLine(TilePos{77,67}, TilePos{100,67}, 1.0, TileType::WALL);
  m_Map.PaintLine(TilePos{71,60}, TilePos{71,75}, 1.0, TileType::WALL);
  m_Map.PaintLine(TilePos{72,73}, TilePos{95,73}, 1.0, TileType::WALL);
  m_Map.PaintLine(TilePos{95,73}, TilePos{95,90}, 1.0, TileType::WALL);
  m_Map.PaintLine(TilePos{71,81}, TilePos{71,100}, 1.0, TileType::WALL);
  m_Map.PaintLine(TilePos{72,81}, TilePos{90,81}, 1.0, TileType::WALL);
  m_Map.PaintLine(TilePos{89,87}, TilePos{89,100}, 1.0, TileType::WALL);
  m_Map.PaintLine(TilePos{84,81}, TilePos{84,96}, 1.0, TileType::WALL);
  m_Map.PaintLine(TilePos{78,87}, TilePos{78,100}, 1.0, TileType::WALL);

  // add some controllable entities 
  m_Entities.clear();
  auto player = std::make_shared<Player>();
  player->SetPosition(m_Map.TileToWorld(TilePos{25, 20}));
  AddEntity(player);

  auto player2 = std::make_shared<Player>();
  player2->SetPosition(m_Map.TileToWorld(TilePos{50, 20}));
  AddEntity(player2);

  for (int i = 0; i < 1; i++)
  {
    for (int j = 0; j < 10; j++)
    {
      auto p = std::make_shared<Player>();
      p->SetPosition(m_Map.TileToWorld(TilePos{10+5*i, 40+5*j}));
      AddEntity(p);
    }
  }

  // select everything - TODO this is just temporary for testing
  for (const auto& entity : m_Entities)
  {
    m_SelectedEntities.push_back(entity);
  }
}

WorldPos PathFindingDemo::GetRandomPosition() const {
  return WorldPos{0.0f, 0.0f}; // totally random!
}



const std::vector<Collision>& PathFindingDemo::GetEntityCollisions()
{
  static std::vector<Collision> m_Collisions;
  m_Collisions.clear();

  for (const auto &entity_A : m_Entities)
  {
    for (const auto &entity_B : m_Entities)
    {
      if (entity_A == entity_B)
        continue;
      if (!entity_A->IsCollidable() || !entity_B->IsCollidable())
        continue;
      if (entity_A->CollidesWith(*entity_B))
      {
        // handle collision logic
        m_Collisions.emplace_back(Collision(entity_A, entity_B));
      }
    }
  }
  return m_Collisions;
}


// Update entity positions, handle collisions
void PathFindingDemo::UpdateWorld() {
  
  float time_delta = 1.0f;
  
  for (auto& entity : m_Entities)
  {
    // calculate the velocity
    auto current_pos = entity->GetPosition();
    double tile_velocity_coeff = m_Map.GetTileVelocityCoeff(current_pos);
    auto next_pos = entity->GetMoveTarget();
    WorldPos velocity = WorldPos{};
    if (next_pos)
    {
      velocity = next_pos.value() - current_pos;
      velocity.Normalize();
      //LOG_DEBUG("I want to move to: ", next_pos.value(),
      //          ", velocity: ", velocity);
    }
    entity->SetActualVelocity(velocity * tile_velocity_coeff);
    
    for (const auto& collision : GetEntityCollisions())
    {
      // TODO this loop is quite "hot", is it good idea to use weak_ptr and promote it?
      auto weak_A = std::get<0>(collision);
      auto weak_B = std::get<1>(collision);
      auto A = weak_A.lock();
      auto B = weak_B.lock();
      if (A == nullptr || B == nullptr)
      {
        continue;
      }
      if (!A->IsMovable())
        continue;
      // modify actual speed
      // LOG_DEBUG("Collision: A is ", A, ", B is ", B);
      auto AB = B->GetPosition() - A->GetPosition();
      A->ZeroActualVelocityInDirection(AB);
      // handle logic
      // TODO
    } 
 
    // update the position
    entity->Update(time_delta);
  }
}

void PathFindingDemo::HandleActions(const std::vector<UserAction> &actions)
{
  for (const auto &action : actions)
  {
    if (action.type == UserAction::Type::EXIT)
    {
      LOG_INFO("Exit requested");
      m_ExitRequested = true;
    }
    else if (action.type == UserAction::Type::SET_MOVE_TARGET)
    {
      WorldPos target_pos = m_Camera.WindowToWorld(action.Argument.position);
      for (auto& selected_entity : m_SelectedEntities)
      {
        LOG_INFO("Calculating path to target: ", target_pos);
        if (auto sp = selected_entity.lock())
        {
          auto path = m_PathFinder->CalculatePath(sp->GetPosition(), target_pos);
          sp->SetPath(path);
          LOG_INFO("Done, path node count: ", path.size());
        } else {
          LOG_INFO("Cannot calculate path for destroyed entity (weak_ptr.lock() failed)");
        }
      }
    }
    else if (action.type == UserAction::Type::SELECT_PATHFINDER)
    {
      using namespace pathfinder;
      PathFinderType type = static_cast<PathFinderType>(action.Argument.number);
      m_PathFinder = pathfinder::utils::create(type, (const Map*)&m_Map);
      LOG_INFO("Switched to path finding method: ", m_PathFinder->GetName());
    }
    else if (action.type == UserAction::Type::CAMERA_PAN)
    {
      const auto& window_pan = action.Argument.position;
      WorldPos world_pan{window_pan.x(), window_pan.y()};
      m_Camera.Pan(world_pan);
      LOG_INFO("Camera pan delta: ", world_pan);
    }
    else if (action.type == UserAction::Type::CAMERA_ZOOM)
    {
      m_Camera.Zoom(action.Argument.float_number);
      LOG_INFO("Camera zoom: ", action.Argument.float_number);
    }
  };
}
