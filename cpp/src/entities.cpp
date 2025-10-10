#include <memory>

#include "entities.hpp"

#include "log.hpp"
#include "math.hpp"
#include "sprite.hpp"

Entity::Entity(WorldPos position) : m_Position(position) {
  LOG_DEBUG("spawning entity at position ", position);
}

std::ostream &operator<<(std::ostream &os, const Entity &obj) {
  static constexpr std::array<std::string_view,
                              static_cast<size_t>(Entity::Type::COUNT)>
      type_name{"NONE", "PLAYER", "TILE"};
  size_t idx = static_cast<size_t>(obj.GetType());
  assert(idx < type_name.size());
  os << type_name[idx];
  return os;
}

void Entity::ZeroActualVelocityInDirection(WorldPos direction) {
  // Vectors e1, e2 form the basis for a local coord system,
  // where e1 is formed by the direction where we want to zero-out
  // the velocity, and e2 is the orthogonal vector.
  // Scalars q1, q2 are coordinates for e1, e2 basis.
  WorldPos e1 = direction.GetNormalized();
  WorldPos e2 = e1.GetOrthogonal();

  // q1 * e1 + q2 * e2 = v, from this follows:
  auto &v = GetActualVelocity();
  float q2 = (v.y() * e1.x() - v.x() * e1.y()) / (e2.y() * e1.x() - e2.x() * e1.y());
  float q1 = (v.x() - q2 * e2.x()) / e1.x();

  // We then zero-out the q1, but only if it's positive - meaning
  // it is aiming in the direction of "direction", not out.
  // (otherwise we're not able to move out from collision with
  // another object)
  if (q1 > 0.0f) {
    SetActualVelocity(q2 * e2);
  }
}

void Entity::Update(float time_delta) {
  m_Position += m_ActualVelocity * time_delta;
}

std::optional<WorldPos> Entity::GetMoveTarget()
{
  auto& path = GetPath();
  if (path.empty()) {
    return {};
  }

  WorldPos current_pos = GetPosition();
  WorldPos next_pos = path.front();

  if (current_pos.DistanceTo(next_pos) > 1.0) {
    // target not reached yet
    return next_pos;
  }
  // target reached, pop it
  //m_MoveQueue.pop();
  path.erase(path.begin());
  // return nothing - if there's next point in the queue,
  // we'll get it in the next iteration
  return {};
}

Player::Player() {
  LOG_DEBUG(".");
  if (m_Sprite == nullptr) {
    LoadResources();
  }
}

Sprite &Player::GetSprite() {
  assert(m_Sprite != nullptr);
  return *m_Sprite;
}

void Player::LoadResources() {
  m_Sprite =
      std::make_unique<Sprite>("resources/player.png", WorldPos{19.0f, 23.0f});
}

std::unique_ptr<Sprite> Player::m_Sprite;
