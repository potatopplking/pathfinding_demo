#pragma once

#include <expected>
#include <string>
#include <vector>

#include "log.hpp"
#include "math.hpp"

class UserAction {
public:
  enum class Type { NONE, EXIT, SET_MOVE_TARGET, SELECT_PATHFINDER, CAMERA_PAN, CAMERA_ZOOM };

  UserAction() : type(Type::NONE), Argument{.number = 0} {}
  UserAction(Type t) : type(t), Argument{.number = 0} {}
  UserAction(Type t, char key) : type(t), Argument{.key = key} {}
  UserAction(Type t, WindowPos v) : type(t), Argument{.position = v} {}
  UserAction(Type t, int arg) : type(t), Argument{.number = arg} {}
  ~UserAction() = default;

  Type type;

  union {
    WindowPos position;
    char key;
    int number;
  } Argument;

  // TODO use std::variant
  //std::variant<WindowPos, char, int> Argument;
};

class UserInput {
public:
  UserInput();
  ~UserInput();

  UserInput(const UserInput &x) = delete;
  UserInput(UserInput &&x) = delete;
  UserInput &operator=(const UserInput &) = delete;
  UserInput &operator=(UserInput &&) = delete;

  std::expected<void, std::string> Init();

  const std::vector<UserAction> &GetActions();

private:
  std::vector<UserAction> m_Actions;
};
