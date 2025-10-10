#pragma once

#include <memory>

#include "pathfindingdemo.hpp"
#include "window.hpp"
#include "user_input.hpp"

class GameLoop {
public:
  GameLoop() = default;
  ~GameLoop() = default;

  GameLoop(const GameLoop&) = delete;
  GameLoop(GameLoop&&) = delete;
  GameLoop& operator=(const GameLoop&) = delete;
  GameLoop& operator=(GameLoop&&) = delete;
 
  void Run();

  void SetGame(std::unique_ptr<PathFindingDemo> x) {
    m_Game = std::move(x);
  }
  void SetWindow(std::unique_ptr<Window> x) { m_Window = std::move(x); }
  void SetUserInput(std::unique_ptr<UserInput> x) {
    m_UserInput = std::move(x);
  }

private:
  void Draw();

  std::unique_ptr<PathFindingDemo> m_Game;
  std::unique_ptr<Window> m_Window;
  std::unique_ptr<UserInput> m_UserInput;
};
