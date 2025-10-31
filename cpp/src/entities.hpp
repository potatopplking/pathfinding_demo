#pragma once

#include <array>
#include <cstdint>
#include <iostream>
#include <memory>
#include <optional>
#include <string_view>

#include "log.hpp"
#include "math.hpp"
#include "pathfinder/base.hpp"
#include "sprite.hpp"

class Entity {
public:
  enum class Type : std::uint8_t {
    NONE,
    PLAYER,
    TILE,
    COUNT // must be last
  };

  Entity(WorldPos position = {0.0f, 0.0f});
  Entity(const Entity &) = delete;
  Entity &operator=(const Entity &) = delete;
  Entity(Entity &&) = delete;
  Entity &operator=(Entity &&) = delete;

  friend std::ostream &operator<<(std::ostream &os, const Entity &obj);

  virtual Sprite &GetSprite() = 0;
  virtual constexpr float GetCollisionRadius() const = 0;
  virtual constexpr Type GetType() const = 0;
  virtual bool IsMovable() const = 0;
  virtual bool IsCollidable() const = 0;

  virtual void Update(float time_delta);

  virtual constexpr float GetCollisionRadiusSquared() const {
    return GetCollisionRadius() * GetCollisionRadius();
  }

  void SetFlagExpired() { m_FlagExpired = true; }
  bool IsFlaggedExpired() const { return m_FlagExpired; }

  const WorldPos &GetPosition() const { return m_Position; }
  void SetPosition(WorldPos new_pos) { m_Position = new_pos; }

  const WorldPos &GetActualVelocity() const { return m_ActualVelocity; }
  const WorldPos &GetRequestedVelocity() const { return m_RequestedVelocity; }
  void SetActualVelocity(const WorldPos &new_velocity) {
    m_ActualVelocity = new_velocity;
  }
  void SetRequestedVelocity(const WorldPos &new_velocity) {
    m_RequestedVelocity = new_velocity;
  }

  void ZeroActualVelocityInDirection(WorldPos direction);

  const pathfinder::Path &GetPath() const { return m_Path; }
  pathfinder::Path &GetPath() { return m_Path; }
  void SetPath(pathfinder::Path &path) { m_Path = path; }
  std::optional<WorldPos> GetMoveTarget();

  bool CollidesWith(const Entity &other) const;

  bool IsCollisionBoxVisible() const { return m_CollisionBoxVisible; }

  void Select() { m_Selected = true; }
  void Deselect() { m_Selected = false; }
  bool IsSelected() const { return m_Selected; }

protected:
  WorldPos m_Position;
  WorldPos m_ActualVelocity;
  WorldPos m_RequestedVelocity;
  pathfinder::Path m_Path;

private:
  bool m_FlagExpired = false;
  bool m_CollisionBoxVisible = true;
  bool m_Selected = false;
};

class Player final : public Entity {
public:
  Player();

  Sprite &GetSprite() override;

  constexpr Entity::Type GetType() const override {
    return Entity::Type::PLAYER;
  }
  constexpr float GetCollisionRadius() const override { return 25.0f; }
  bool IsMovable() const override { return true; }
  bool IsCollidable() const override { return true; }

private:
  void LoadResources();
  static std::unique_ptr<Sprite> m_Sprite;
};
