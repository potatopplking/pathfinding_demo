#pragma once

#include <expected>
#include <string>
#include <vector>

#include "log.hpp"
#include "math.hpp"

class UserAction {
public:
  enum class Type { NONE, EXIT, SET_MOVE_TARGET, SELECT_PATHFINDER };

  UserAction() = default;
  UserAction(Type t) : type(t) {}
  UserAction(Type t, char key) : type(t), Argument{.key = key} {}
  UserAction(Type t, WorldPos v) : type(t), Argument{.position = v} {}
  UserAction(Type t, int arg) : type(t), Argument{.number = arg} {}
  ~UserAction() = default;

  Type type;

  union {
    WorldPos position;
    char key;
    int number;
  } Argument;
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
