#pragma once

#include <cassert>
#include <cmath>
#include <concepts>
#include <initializer_list>
#include <iostream>
#include <utility>

constexpr double EQUALITY_LIMIT = 1e-6;

template <typename T> struct Vec2D {
public:
  Vec2D() = default;
  ~Vec2D() = default;

  template <typename U>
  Vec2D(Vec2D<U> other) {
    this->x = static_cast<T>(other.x);
    this->y = static_cast<T>(other.y);
  }

  Vec2D& operator+=(const Vec2D &other) {
    x += other.x;
    y += other.y;
    return *this;
  }

  template <typename U>
  requires std::is_arithmetic_v<U>
  Vec2D& operator/=(U k) {
    this->x /= static_cast<T>(k);
    this->y /= static_cast<T>(k);
    return *this;
  }

  friend Vec2D operator+(const Vec2D &a, const Vec2D &b) {
    return Vec2D{a.x + b.x, a.y + b.y};
  }
  
  friend Vec2D operator-(const Vec2D &a, const Vec2D &b) {
    return Vec2D{a.x - b.x, a.y - b.y};
  }

  template <typename U>
    requires std::is_arithmetic_v<U>
  friend Vec2D operator*(U k, const Vec2D &v)
  {
    return Vec2D{k * v.x, k * v.y};
  }

  template <typename U>
    requires std::is_arithmetic_v<U>
  friend Vec2D operator/(const Vec2D &v, U k)
  {
    return Vec2D{v.x / k, v.y / k};
  }

  friend bool operator==(const Vec2D &a, const Vec2D &b) {
    if constexpr (std::is_integral_v<T>) {
      return a.x == b.x && a.y == b.y;
    } else if constexpr (std::is_floating_point_v<T>) {
      return a.distance(b) < EQUALITY_LIMIT;
    } else {
      static_assert("Unhandled comparison");
    }
  }

  Vec2D operator*(float b) const { return Vec2D{b * x, b * y}; }

  T distance_squared(const Vec2D &other) const {
    T dx = x - other.x;
    T dy = y - other.y;
    return dx * dx + dy * dy;
  }

  T distance(const Vec2D &other) const
    requires std::floating_point<T>
  {
    return sqrt(distance_squared(other));
  }

  void normalize()
    requires std::floating_point<T>
  {
    auto length = sqrt(x * x + y * y);
    if (length < EQUALITY_LIMIT) {
      x = y = 0;
    } else {
      x /= length;
      y /= length;
    }
  }

  Vec2D normalized()
    requires std::floating_point<T>
  {
    Vec2D v(*this);
    v.normalize();
    return v;
  }

  Vec2D orthogonal() const
  {
    Vec2D v(*this);
    
    std::swap(v.x, v.y);
    v.x = -v.x;
    return v;
  }

  template <typename U> Vec2D(std::initializer_list<U> list) {
    assert(list.size() == 2);
    auto first_element = *list.begin();
    auto second_element = *(list.begin() + 1);
    x = static_cast<T>(first_element);
    y = static_cast<T>(second_element);
  }

  T x, y;

  friend std::ostream &operator<<(std::ostream &os, const Vec2D &obj) {
    os << "( " << obj.x << ", " << obj.y << ")";
    return os;
  }
};

using TilePos = Vec2D<int>;
using WorldPos = Vec2D<float>;

struct TilePosHash {
    std::size_t operator()(const TilePos& p) const noexcept {
        std::size_t h1 = std::hash<int>{}(p.x);
        std::size_t h2 = std::hash<int>{}(p.y);
        return h1 ^ (h2 + 0x9e3779b9 + (h1<<6) + (h1>>2));
    }
};

