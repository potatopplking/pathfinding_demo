#include <SDL3/SDL.h>
#include <expected>
#include <map>
#include <string>
#include <vector>

#include "user_input.hpp"

#include "log.hpp"
#include "math.hpp"

UserInput::UserInput()
    : // pre-alloc some space
      m_Actions(10) {
  LOG_DEBUG(".");
};

UserInput::~UserInput() { LOG_DEBUG("."); };

std::expected<void, std::string> UserInput::Init() { return {}; }

const std::vector<UserAction> &UserInput::GetActions() {
  m_Actions.clear();
  static WorldPos move_direction = {0.0f, 0.0f};
  static bool send_move_action = false;
  SDL_Event event;

  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP) {
      bool key_down = event.type == SDL_EVENT_KEY_DOWN ? true : false;
      SDL_KeyboardEvent kbd_event = event.key;
      if (kbd_event.repeat) {
        // SDL repeats KEY_DOWN if key is held down, we ignore that
        continue;
      }
      LOG_DEBUG("Key '", static_cast<char>(kbd_event.key),
                key_down ? "' down" : "' up");

      switch (kbd_event.key) {
      case 'q':
        m_Actions.emplace_back(UserAction::Type::EXIT);
        // further processing of inputs is not needed
        return m_Actions;
      case 'w':
      case 's':
      case 'a':
      case 'd':
      case SDLK_UP:
      case SDLK_DOWN:
      case SDLK_LEFT:
      case SDLK_RIGHT: {
        static std::map<char, WorldPos> move_base{
            {'w', {0.0, 1.0}},
            {'s', {0.0, -1.0}},
            {'a', {1.0, 0.0}},
            {'d', {-1.0, 0.0}},
            {static_cast<char>(SDLK_UP), {0.0, 1.0}},
            {static_cast<char>(SDLK_DOWN), {0.0, -1.0}},
            {static_cast<char>(SDLK_LEFT), {1.0, 0.0}},
            {static_cast<char>(SDLK_RIGHT), {-1.0, 0.0}},
        };

        float direction = key_down ? -1.0f : 1.0f;
        send_move_action = true;
        move_direction += move_base[kbd_event.key] * direction;
        break;
      }
      case SDLK_SPACE:
        if (key_down)
          m_Actions.emplace_back(UserAction::Type::FIRE);
        break;
      default:
        LOG_INFO("Key '", static_cast<char>(kbd_event.key), "' not mapped");
        break;
      }
    } else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
      SDL_MouseButtonEvent mouse_event = event.button;
      LOG_DEBUG("Mouse down: ", mouse_event.x, ", ", mouse_event.y);
      m_Actions.emplace_back(UserAction::Type::MOVE_TARGET,
                             WorldPos{mouse_event.x, mouse_event.y});
    } else {
      // TODO uncomment, for now too much noise
      // LOG_WARNING("Action not processed");
    }
  }
  if (send_move_action) {
    m_Actions.emplace_back(UserAction::Type::MOVE, move_direction.normalized());
    send_move_action = false;
  }
  return m_Actions;
}
