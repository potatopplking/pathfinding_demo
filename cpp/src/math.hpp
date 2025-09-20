#pragma once

#include <cassert>
#include <cmath>
#include <concepts>
#include <iostream>

template <typename T> struct Vec2D {
public:
  Vec2D() = default;
  ~Vec2D() = default;

  Vec2D &operator+=(const Vec2D &other) {
    x += other.x;
    y += other.y;
    return *this;
  }

  friend Vec2D operator+(const Vec2D &a, const Vec2D &b) {
    return Vec2D{a.x + b.x, a.y + b.y};
  }
  
  friend Vec2D operator-(const Vec2D &a, const Vec2D &b) {
    return Vec2D{a.x - b.x, a.y - b.y};
  }

  friend Vec2D operator*(float k, const Vec2D &v) {
    return Vec2D{k * v.x, k * v.y};
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
    if (length < 1e-6) {
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

  Vec2D orthogonal()
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
