#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>
#include <concepts>
#include <initializer_list>
#include <iostream>
#include <numeric>
#include <ranges>
#include <utility>

template <typename T>
  requires std::floating_point<T>
static inline bool equalEpsilon(const T &a, const T &b) {
  constexpr auto epsilon = []() {
    if constexpr (std::is_same_v<T, float>) {
      return T{1e-5};
    } else {
      return T{1e-12}; // double, long double
    }
  }();
  if (a == b) {
    // handle special cases: bit equality, Inf...
    return true;
  }
  return std::abs(a - b) < epsilon;
}

struct Any {};

template <typename T, size_t N, typename Tag = Any> class vec {
public:
  vec() : m_Array{} {}

  template <typename... ArgsT>
    requires(std::same_as<ArgsT, T> && ...) && (sizeof...(ArgsT) == N)
  vec(ArgsT... args) : m_Array{args...} {}

  const T &operator[](size_t index) const {
    // we leave run-time checks to the underlying std::array
    return m_Array[index];
  }

  T &operator[](size_t index) {
    // we leave run-time checks to the underlying std::array
    return m_Array[index];
  }

  friend std::ostream &operator<<(std::ostream &os, const vec &obj) {
    os << "( ";
    for (const auto &element : obj.m_Array) {
      os << element << " ";
    }
    os << ")";
    return os;
  }

  //
  // binary operators
  //

  friend bool operator==(const vec &a, const vec &b)
    requires (std::is_integral_v<T>)
  {
    return std::ranges::equal(a.m_Array, b.m_Array);
  }

  friend bool operator==(const vec &a, const vec &b)
    requires (std::is_floating_point_v<T>)
  {
    for (const auto &[u, v] : std::views::zip(a.m_Array, b.m_Array)) {
      if (!equalEpsilon(u, v)) {
        return false;
      }
    }
    return true;
  }

  friend bool operator!=(const vec &a, const vec &b) { return !(a == b); }

  friend vec operator+(const vec &a, const vec &b) {
    vec<T, N, Tag> c;
    std::ranges::transform(a.m_Array, b.m_Array, c.m_Array.begin(),
                           std::plus{});
    return c;
  }

  friend vec operator-(const vec &a, const vec &b) {
    vec<T, N, Tag> c;
    std::ranges::transform(a.m_Array, b.m_Array, c.m_Array.begin(),
                           std::minus{});
    return c;
  }

  friend vec operator*(const vec &a, const T &scalar) {
    vec<T, N, Tag> c;
    std::ranges::transform(a.m_Array, std::views::repeat(scalar),
                           c.m_Array.begin(), std::multiplies{});
    return c;
  }

  friend vec operator*(const T &scalar, const vec &a) { return a * scalar; }

  friend vec operator/(const vec &a, const T &scalar) {
    vec<T, N, Tag> c;
    std::ranges::transform(a.m_Array, std::views::repeat(scalar),
                           c.m_Array.begin(), std::divides{});
    return c;
  }

  //
  // compound-assignment operators
  //

  vec &operator+=(const vec &b) {
    vec &a = *this;
    std::ranges::transform(a.m_Array, b.m_Array, a.m_Array.begin(),
                           std::plus{});
    return a;
  }

  vec &operator-=(const vec &b) {
    vec &a = *this;
    std::ranges::transform(a.m_Array, b.m_Array, a.m_Array.begin(),
                           std::minus{});
    return a;
  }

  //
  // Utility functions
  //

  T LengthSquared() const {
    return std::transform_reduce(m_Array.begin(), m_Array.end(), T{},
                                 std::plus{}, [](T x) { return x * x; });
  }

  T Length() const { return std::sqrt(LengthSquared()); }

  T DistanceTo(const vec &b) const {
    const vec &a = *this;
    return (a - b).Length();
  }

  //
  // In-place vector operations
  //

  void Normalize() {
    T length = Length();
    if (equalEpsilon(length, T{0}))
      return;
    std::ranges::transform(m_Array, std::views::repeat(length), m_Array.begin(),
                           std::divides{});
  }

  //
  // Methods returning new object
  //

  vec GetNormalized() const {
    vec tmp = *this;
    tmp.Normalize();
    return tmp;
  }

  vec GetOrthogonal() const
    requires(N == 2)
  {
    vec tmp = *this;

    std::swap(tmp.m_Array[0], tmp.m_Array[1]);
    tmp.m_Array[0] *= -1;

    return tmp;
  }

  //
  // Helpers
  //

  const T &x() const
    requires(N >= 1)
  {
    return m_Array[0];
  }

  T &x()
    requires(N >= 1)
  {
    return m_Array[0];
  }

  const T &y() const
    requires(N >= 2)
  {
    return m_Array[1];
  }

  T &y()
    requires(N >= 2)
  {
    return m_Array[1];
  }

  const T &z() const
    requires(N >= 3)
  {
    return m_Array[2];
  }

  T &z()
    requires(N >= 3)
  {
    return m_Array[2];
  }

private:
  std::array<T, N> m_Array;
};

//
// Aliases
//

using vec2 = vec<float, 2>;
using vec3 = vec<float, 3>;
using vec4 = vec<float, 4>;
using dvec2 = vec<double, 2>;
using dvec3 = vec<double, 3>;
using dvec4 = vec<double, 4>;
using ivec2 = vec<std::int32_t, 2>;
using ivec3 = vec<std::int32_t, 3>;
using ivec4 = vec<std::int32_t, 4>;
using uvec2 = vec<std::uint32_t, 2>;
using uvec3 = vec<std::uint32_t, 3>;
using uvec4 = vec<std::uint32_t, 4>;

// tags for differentiating between domains
struct WorldTag {};
struct WindowTag {};
struct TileTag {};

// types for each domain
using WorldPos = vec<float, 2, WorldTag>;
using WindowPos = vec<float, 2, WindowTag>;
using TilePos = vec<int32_t, 2, TileTag>;

//
// Utils
//

struct TilePosHash {
  std::size_t operator()(const TilePos &p) const noexcept {
    std::size_t h1 = std::hash<int>{}(p.x());
    std::size_t h2 = std::hash<int>{}(p.y());
    return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
  }
};

// old stuff - TODO delete

// constexpr double EQUALITY_LIMIT = 1e-6;
// template <typename T> struct Vec2D {
// public:
//   Vec2D() = default;
//   ~Vec2D() = default;
//
//   template <typename U>
//   Vec2D(Vec2D<U> other) {
//     this->x = static_cast<T>(other.x);
//     this->y = static_cast<T>(other.y);
//   }
//
//   Vec2D& operator+=(const Vec2D &other) {
//     x += other.x;
//     y += other.y;
//     return *this;
//   }
//
//   template <typename U>
//   requires std::is_arithmetic_v<U>
//   Vec2D& operator/=(U k) {
//     this->x /= static_cast<T>(k);
//     this->y /= static_cast<T>(k);
//     return *this;
//   }
//
//   friend Vec2D operator+(const Vec2D &a, const Vec2D &b) {
//     return Vec2D{a.x + b.x, a.y + b.y};
//   }
//
//   friend Vec2D operator-(const Vec2D &a, const Vec2D &b) {
//     return Vec2D{a.x - b.x, a.y - b.y};
//   }
//
//   template <typename U>
//     requires std::is_arithmetic_v<U>
//   friend Vec2D operator*(U k, const Vec2D &v)
//   {
//     return Vec2D{k * v.x, k * v.y};
//   }
//
//   template <typename U>
//     requires std::is_arithmetic_v<U>
//   friend Vec2D operator/(const Vec2D &v, U k)
//   {
//     return Vec2D{v.x / k, v.y / k};
//   }
//
//   friend bool operator==(const Vec2D &a, const Vec2D &b) {
//     if constexpr (std::is_integral_v<T>) {
//       return a.x == b.x && a.y == b.y;
//     } else if constexpr (std::is_floating_point_v<T>) {
//       return a.distance(b) < EQUALITY_LIMIT;
//     } else {
//       static_assert("Unhandled comparison");
//     }
//   }
//
//   Vec2D operator*(float b) const { return Vec2D{b * x, b * y}; }
//
//   T distance_squared(const Vec2D &other) const {
//     T dx = x - other.x;
//     T dy = y - other.y;
//     return dx * dx + dy * dy;
//   }
//
//   T distance(const Vec2D &other) const
//     requires std::floating_point<T>
//   {
//     return sqrt(distance_squared(other));
//   }
//
//   void normalize()
//     requires std::floating_point<T>
//   {
//     auto length = sqrt(x * x + y * y);
//     if (length < EQUALITY_LIMIT) {
//       x = y = 0;
//     } else {
//       x /= length;
//       y /= length;
//     }
//   }
//
//   Vec2D normalized()
//     requires std::floating_point<T>
//   {
//     Vec2D v(*this);
//     v.normalize();
//     return v;
//   }
//
//   Vec2D orthogonal() const
//   {
//     Vec2D v(*this);
//
//     std::swap(v.x, v.y);
//     v.x = -v.x;
//     return v;
//   }
//
//   template <typename U> Vec2D(std::initializer_list<U> list) {
//     assert(list.size() == 2);
//     auto first_element = *list.begin();
//     auto second_element = *(list.begin() + 1);
//     x = static_cast<T>(first_element);
//     y = static_cast<T>(second_element);
//   }
//
//   T x, y;
//
//   friend std::ostream &operator<<(std::ostream &os, const Vec2D &obj) {
//     os << "( " << obj.x << ", " << obj.y << ")";
//     return os;
//   }
// };
