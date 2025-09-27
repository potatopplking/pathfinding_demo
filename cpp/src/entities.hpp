#pragma once

#include <array>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string_view>

#include "math.hpp"
#include "sprite.hpp"
#include "log.hpp"

class Entity {
public:
  enum class Type : std::uint8_t {
    NONE,
    PLAYER,
    WALL,
    TILE,
    COUNT // must be last
  };

  Entity(Vec2D<float> position = {0.0f, 0.0f}) : m_Position(position) {
    LOG_DEBUG("spawning entity at position ", position);
  }

  friend std::ostream &operator<<(std::ostream &os, const Entity &obj) {
    static constexpr std::array<std::string_view,
                                static_cast<size_t>(Entity::Type::COUNT)>
        type_name{"NONE", "PLAYER", "WALL", "TILE"};
    size_t idx = static_cast<size_t>(obj.GetType());
    assert(idx < type_name.size());
    os << type_name[idx];
    return os;
  }

  virtual Sprite &GetSprite() = 0;
  virtual constexpr float GetCollisionRadius() const = 0;
  virtual constexpr float GetCollisionRadiusSquared() {
    return GetCollisionRadius() * GetCollisionRadius();
  }

  virtual constexpr Type GetType() const = 0;
  void SetFlagExpired() { m_FlagExpired = true; }
  bool IsFlaggedExpired() { return m_FlagExpired; }

  const Vec2D<float> &GetPosition() const { return m_Position; }
  void SetPosition(Vec2D<float> new_pos) { m_Position = new_pos; }

  const Vec2D<float> &GetActualVelocity() const { return m_ActualVelocity; }
  const Vec2D<float> &GetRequestedVelocity() const {
    return m_RequestedVelocity;
  }
  void SetActualVelocity(const Vec2D<float> &new_velocity) {
    m_ActualVelocity = new_velocity;
  }
  void SetRequestedVelocity(const Vec2D<float> &new_velocity) {
    m_RequestedVelocity = new_velocity;
  }

  void ZeroActualVelocityInDirection(Vec2D<float> direction) {
    // Vectors e1, e2 form the basis for a local coord system,
    // where e1 is formed by the direction where we want to zero-out
    // the velocity, and e2 is the orthogonal vector.
    // Scalars q1, q2 are coordinates for e1, e2 basis.
    Vec2D<float> e1 = direction.normalized();
    Vec2D<float> e2 = e1.orthogonal();

    // q1 * e1 + q2 * e2 = v, from this follows:
    auto &v = GetActualVelocity();
    float q2 = (v.y * e1.x - v.x * e1.y) / (e2.y * e1.x - e2.x * e1.y);
    float q1 = (v.x - q2 * e2.x) / e1.x;

    // We then zero-out the q1, but only if it's positive - meaning
    // it is aiming in the direction of "direction", not out.
    // (otherwise we're not able to move out from collision with
    // another object)
    if (q1 > 0.0f) {
      SetActualVelocity(q2 * e2);
    }
  }

  virtual void Update(float time_delta) {
    m_Position += m_ActualVelocity * time_delta;
  }

  virtual bool IsMovable() const = 0;
  virtual bool IsCollidable() const = 0;

protected:
  Vec2D<float> m_Position;
  Vec2D<float> m_ActualVelocity;
  Vec2D<float> m_RequestedVelocity;

private:
  bool m_FlagExpired = false;
  static constexpr float m_CollisionRadiusSq = 1000.0f;
};

class Wall final : public Entity {
public:
  Wall(Vec2D<float> pos = {0.0f, 0.0f}) : Entity(pos) {
    LOG_DEBUG(".");
    if (m_Sprite == nullptr) {
      LoadResources();
    }
  }
  Wall(const Wall &x) = delete;
  Wall(Wall &&x) = delete;

  Sprite &GetSprite() override {
    assert(m_Sprite != nullptr);
    return *m_Sprite;
  }
  constexpr Entity::Type GetType() const override { return Entity::Type::WALL; }
  constexpr float GetCollisionRadius() const override { return 50.0f; }
  bool IsMovable() const override { return false; }
  bool IsCollidable() const override { return true; }

private:
  void LoadResources() {
    m_Sprite = std::make_unique<Sprite>("resources/wall.png",
                                        Vec2D<float>{50.0f, 50.0f});
  }
  static std::unique_ptr<Sprite> m_Sprite;
};
std::unique_ptr<Sprite> Wall::m_Sprite;

class Player final : public Entity {
public:
  Player() {
    LOG_DEBUG(".");
    if (m_Sprite == nullptr) {
      LoadResources();
    }
  }
  Player(const Player &x) = delete;
  Player(Player &&x) = delete;

  Sprite &GetSprite() override {
    assert(m_Sprite != nullptr);
    return *m_Sprite;
  }

  constexpr Entity::Type GetType() const override {
    return Entity::Type::PLAYER;
  }
  constexpr float GetCollisionRadius() const override { return 50.0f; }
  bool IsMovable() const override { return true; }
  bool IsCollidable() const override { return true; }

private:
  void LoadResources() {
    m_Sprite = std::make_unique<Sprite>("resources/player.png",
                                        Vec2D<float>{38.0f, 46.0f});
  }
  static std::unique_ptr<Sprite> m_Sprite;
};

std::unique_ptr<Sprite> Player::m_Sprite;


