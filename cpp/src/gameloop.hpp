#pragma once

#include <chrono>
#include <memory>
#include <thread>

#include "pathfindingdemo.hpp"
#include "window.hpp"
#include "user_input.hpp"
#include "log.hpp"

class GameLoop {
public:
  GameLoop() = default;

  void Run() {
    LOG_INFO("Running the game");
    while (!m_Game->IsExitRequested()) {
      m_Game->HandleActions(m_UserInput->GetActions());
      m_Game->UpdatePlayerVelocity();

      m_Window->ClearWindow();

      // draw the map (terrain tiles)
      const Map &map = m_Game->GetMap();
      const auto &tiles = map.GetMapTiles();
      for (size_t row = 0; row < tiles.size(); row++) {
        for (size_t col = 0; col < tiles[row].size(); col++) {
          // LOG_DEBUG("Drawing rect (", row, ", ", col, ")");
          m_Window->DrawRect(
              map.TileToWorld(TilePos{row, col}),
              map.GetTileSize(), tiles[row][col]->R, tiles[row][col]->G,
              tiles[row][col]->B, tiles[row][col]->A);
        }
      }

      // draw all the entities (player etc)
      for (auto &entity : m_Game->GetEntities()) {
        m_Window->DrawSprite(entity->GetPosition(), entity->GetSprite());
      }

      m_Window->Flush();
      // TODO measure fps
      std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
  }

  inline void SetGame(std::unique_ptr<PathFindingDemo> x) {
    m_Game = std::move(x);
  }
  inline void SetWindow(std::unique_ptr<Window> x) { m_Window = std::move(x); }
  inline void SetUserInput(std::unique_ptr<UserInput> x) {
    m_UserInput = std::move(x);
  }

private:
  std::unique_ptr<PathFindingDemo> m_Game;
  std::unique_ptr<Window> m_Window;
  std::unique_ptr<UserInput> m_UserInput;
};
