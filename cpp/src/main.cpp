#include "gameloop.hpp"
#include "log.hpp"
#include "pathfindingdemo.hpp"
#include "user_input.hpp"
#include "window.hpp"
#include <memory>

int main() {
  constexpr int error = -1;

  /*
   * Initialize the input/output system
   */

  auto window = std::make_unique<Window>(640, 480);
  if (auto initialized = window->Init(); !initialized) {
    LOG_ERROR(initialized.error());
    return error;
  }

  auto user_input = std::make_unique<UserInput>();
  if (auto initialized = user_input->Init(); !initialized) {
    LOG_ERROR(initialized.error());
    return error;
  }

  /*
   * Initialize the map and run the pathfinding demo
   */

  auto demo = std::make_unique<PathFindingDemo>(10, 10);
  demo->CreateMap();

  auto game_loop = GameLoop{};
  game_loop.SetWindow(std::move(window));
  game_loop.SetUserInput(std::move(user_input));
  game_loop.SetGame(std::move(demo));
  game_loop.Run();
  return 0;
}
