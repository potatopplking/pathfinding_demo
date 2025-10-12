#include <memory>
#include <thread>

#include "gameloop.hpp"

#include "log.hpp"
#include "math.hpp"
#include "pathfinder/base.hpp"
#include "pathfindingdemo.hpp"
#include "user_input.hpp"
#include "window.hpp"

void GameLoop::Draw() {
  // draw the map (terrain tiles)
  const Map &map = m_Game->GetMap();
  const auto &tiles = map.GetMapTiles();
  for (size_t row = 0; row < tiles.size(); row++) {
    for (size_t col = 0; col < tiles[row].size(); col++) {
      const auto &camera = m_Game->GetCamera();
      const auto &position = camera.WorldToWindow(map.TileEdgeToWorld(
          TilePos{static_cast<int32_t>(row), static_cast<int32_t>(col)}));
      const auto &size = camera.WorldToWindowSize(map.GetTileSize());
      // LOG_DEBUG("Drawing rect (", row, ", ", col, ")");
      m_Window->DrawFilledRect(position, size, tiles[row][col]->R, tiles[row][col]->G,
                         tiles[row][col]->B, tiles[row][col]->A);
    }
  }

  // draw the path, if it exists

  for (const auto& entity : m_Game->GetEntities())
  {
    WorldPos start_pos = entity->GetPosition();
    for (const auto &next_pos : entity->GetPath())
    {
      const auto &camera = m_Game->GetCamera();
      m_Window->DrawLine(camera.WorldToWindow(start_pos),
                         camera.WorldToWindow(next_pos));
      start_pos = next_pos;
    }
  }

  // draw all the entities (player etc)
  for (auto &entity : m_Game->GetEntities()) {
    const auto &camera = m_Game->GetCamera();
    auto entity_pos = camera.WorldToWindow(entity->GetPosition());
    m_Window->DrawSprite(entity_pos,
                         entity->GetSprite(),
                         camera.GetZoom());
    if (entity->IsCollisionBoxVisible())
    {
      float collision_radius = camera.WorldToWindowSize(entity->GetCollisionRadius());
      m_Window->DrawCircle(entity_pos, collision_radius);
    }
  }

  // draw the selection box, if present
  if (m_Game->IsSelectionBoxActive())
  {
    const auto& [corner_pos, size] = m_Game->GetSelectionBoxPosSize();
    m_Window->DrawRect(corner_pos, size, 200, 20, 20);
  }


}

// TODO rethink coupling and dependencies in the game loop class

void GameLoop::Run() {
  LOG_INFO("Running the game");
  while (!m_Game->IsExitRequested()) {
    m_Game->HandleActions(m_UserInput->GetActions());
    m_Game->UpdateWorld();

    // TODO measure fps, draw only if delay for target fps was reached
    // or create a separate thread for drawing
    m_Window->ClearWindow();
    Draw();
    m_Window->Flush();

    std::this_thread::sleep_for(std::chrono::milliseconds(30));
  }
}
