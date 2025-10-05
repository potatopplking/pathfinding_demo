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

void UserInput::GetActions_mouse(const SDL_Event& event)
{
  static bool mouse_pan = false;

  if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
    SDL_MouseButtonEvent mouse_event = event.button;
    MouseButton button = static_cast<MouseButton>(mouse_event.button);
    if (button == MouseButton::LEFT) {
      LOG_DEBUG("Mouse down: ", mouse_event.x, ", ", mouse_event.y);
      m_Actions.emplace_back(UserAction::Type::SET_MOVE_TARGET,
                             WindowPos{mouse_event.x, mouse_event.y});
    } else if (button == MouseButton::MIDDLE) {
      mouse_pan = true;
    }
  } else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
    SDL_MouseButtonEvent mouse_event = event.button;
    MouseButton button = static_cast<MouseButton>(mouse_event.button);
    if (button == MouseButton::MIDDLE) {
      mouse_pan = false;
    }
  } else if (event.type == SDL_EVENT_MOUSE_MOTION) {
    SDL_MouseMotionEvent motion_event = event.motion;
    if (mouse_pan) {
      m_Actions.emplace_back(UserAction::Type::CAMERA_PAN,
                             WindowPos{motion_event.xrel, motion_event.yrel});
      
    }
  }
}

void UserInput::GetActions_kbd(const SDL_Event& event)
{
      bool key_down = event.type == SDL_EVENT_KEY_DOWN ? true : false;
      SDL_KeyboardEvent kbd_event = event.key;
      if (kbd_event.repeat) {
        // SDL repeats KEY_DOWN if key is held down, we ignore that
        return;
      }
      LOG_DEBUG("Key '", static_cast<char>(kbd_event.key),
                key_down ? "' down" : "' up");

      switch (kbd_event.key) {
      case 'q':
        m_Actions.emplace_back(UserAction::Type::EXIT);
        return;
      case '1':
      case '2':
      case '3':
      case '4':
        if (key_down) {
          int selection = kbd_event.key - '0';
          m_Actions.emplace_back(UserAction::Type::SELECT_PATHFINDER, selection);
          LOG_INFO("Pathfinder selected: ", selection);
        }
       break;
      default:
        LOG_INFO("Key '", static_cast<char>(kbd_event.key), "' not mapped");
        break;
      }
}

const std::vector<UserAction>& UserInput::GetActions() {
  m_Actions.clear();
  SDL_Event event;
  
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_EVENT_KEY_DOWN
        || event.type == SDL_EVENT_KEY_UP)
    {
      GetActions_kbd(event);
    }
    else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN
        || event.type == SDL_EVENT_MOUSE_BUTTON_UP
        || event.type == SDL_EVENT_MOUSE_MOTION)
    {
      GetActions_mouse(event);       
    }
    else
    {
      // TODO uncomment, for now too much noise
      // LOG_WARNING("Action not processed");
    }
  }
  return m_Actions;
}
