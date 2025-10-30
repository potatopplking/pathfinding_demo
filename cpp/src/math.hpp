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
#include <functional>

#ifdef _WIN32
#include <numbers>
#define M_PI std::numbers::pi
// TODO use std::numbers::pi instead of M_PI
#endif

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

  // Friend declaration for move constructor from different tag types
  template <typename U, size_t M, typename OtherTag>
  friend class vec;

public:
  vec() : m_Array{} {}

  template <typename... ArgsT>
    requires(std::same_as<ArgsT, T> && ...) && (sizeof...(ArgsT) == N)
  vec(ArgsT... args) : m_Array{args...} {}

  vec(std::array<T, N> array) : m_Array{array} {}

  template<typename OtherTag>
  vec(vec<T, N, OtherTag>&& other) : m_Array{std::move(other.m_Array)} {}

  //
  // Access to elements & data
  //

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

  std::array<T,N>& Data() { return m_Array; }

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

  friend vec operator+(const vec& a, T b)
  {
    vec<T, N, Tag> c;
    std::ranges::transform(a.m_Array, std::views::repeat(b), c.m_Array.begin(),
                           std::plus{});
    return c;
  }

  friend vec operator-(const vec &a, const vec &b) {
    vec<T, N, Tag> c;
    std::ranges::transform(a.m_Array, b.m_Array, c.m_Array.begin(),
                           std::minus{});
    return c;
  }

  friend vec operator-(const vec& a, T b)
  {
    vec<T, N, Tag> c;
    std::ranges::transform(a.m_Array, std::views::repeat(b), c.m_Array.begin(),
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

  friend vec operator/(const vec &a, const vec& b) {
    vec<T, N, Tag> c;
    std::ranges::transform(a.m_Array, b.m_Array,
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

  vec& operator/=(float scalar)
  {
    vec& a = *this;
    auto b = std::views::repeat(scalar);
    std::ranges::transform(a.m_Array, b, a.m_Array.begin(), std::divides{});
    // TODO check all of this, could be done better with views instead of ranges?
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

  T DistanceSquared(const vec &b) const {
    const vec &a = *this;
    return (a - b).LengthSquared();
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

  static T DotProduct(const vec& a, const vec& b)
  {
    return std::inner_product(
        a.m_Array.begin(), a.m_Array.end(), b.m_Array.begin(), 
        T{}, std::plus{}, std::multiplies{});
  }

  T DotProduct(const vec& b) const
  {
    const auto& a = *this;
    return DotProduct(a, b);
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

  template <typename TargetTag>
  vec<T,N,TargetTag> ChangeTag() const &
  {
    return vec<T,N,TargetTag>(m_Array);
  }

  template <typename TargetTag>
  vec<T,N,TargetTag> ChangeTag() &&
  {
    return vec<T,N,TargetTag>(std::move(*this));
  }

  // Structured binding support for N == 2
  template<size_t I>
  const T& get() const
    requires(N == 2 && I < 2)
  {
    return m_Array[I];
  }

  template<size_t I>
  T& get()
    requires(N == 2 && I < 2)
  {
    return m_Array[I];
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
struct WorldPosTag {};
struct WorldSizeTag {};
struct WindowPosTag {};
struct WindowSizeTag {};
struct TilePosTag {};
struct TileSizeTag {};

// types for each domain
using WorldPos = vec<float, 2, WorldPosTag>;
using WindowPos = vec<float, 2, WindowPosTag>;
using TilePos = vec<int32_t, 2, TilePosTag>;
// Size
using WorldSize = vec<float, 2, WorldSizeTag>;
using WindowSize = vec<float, 2, WindowSizeTag>;
using TileSize = vec<int32_t, 2, TileSizeTag>;

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

//
// Matrix
//

// Collumn major square matrix
template <typename T, size_t N, typename Tag = Any>
class Matrix {

using vec_type = vec<T, N, Tag>;

public:
    Matrix() = default;

    // Initialization using flat array of N*N elements
    template <typename Tarr, size_t M>
    requires (M == N*N && std::same_as<Tarr, T>)
    Matrix(std::array<Tarr,M> array) : m_Array{}
    {
        std::size_t idx = 0;
        for (auto col : array | std::views::chunk(N))
        {
            std::ranges::copy(col, m_Array[idx++].Data().begin());
        }
    }

    const vec_type& operator[](size_t index) const { return m_Array[index]; }
    vec_type& operator[](size_t index) { return m_Array[index]; }

    friend std::ostream &operator<<(std::ostream &os, const Matrix &obj)
    {
        os << "( ";
        for (const auto &element : obj.m_Array) {
        os << element << " ";
        }
        os << ")";
        return os;
    }


    friend Matrix operator+(const Matrix& A, const Matrix& B)
    {
        Matrix C;
        std::ranges::transform(A.m_Array, B.m_Array, C.m_Array.begin(), std::plus{});
        return C;
    }

    friend Matrix operator-(const Matrix& A, const Matrix& B)
    {
        Matrix C;
        std::ranges::transform(A.m_Array, B.m_Array, C.m_Array.begin(), std::minus{});
        return C;
    }

    friend Matrix operator*(const Matrix& A, const Matrix& B)
    {
        Matrix C;

        for (size_t i = 0; i < N; i++)
        {
            for (size_t j = 0; j < N; j++)
            {
                T sum = 0;
                for (size_t k = 0; k < N; ++k) sum += A[i][k] * B[k][j];
                C[i][j] = sum;
            }
        }
        return C;
    }

    friend vec_type operator*(const Matrix& A, const vec_type& b)
    {
        // we assume that b is row vector
        vec_type c;
        for (size_t i = 0; i < N; i++)
        {
            c[i] = b.DotProduct(A[i]);
        }
        return c;
    }

    static constexpr Matrix Eye()
    {
        Matrix E;
        for (size_t i = 0; i < N; i++)
        {
            E[i][i] = T{1};
        }
        return E;
    }


private:
    std::array<vec_type, N> m_Array;
};

// Structured binding support for vec<T, 2, Tag>
namespace std {
  template<typename T, typename Tag>
  struct tuple_size<vec<T, 2, Tag>> : integral_constant<size_t, 2> {};

  template<size_t I, typename T, typename Tag>
  struct tuple_element<I, vec<T, 2, Tag>> {
    using type = T;
  };
}

// ADL-based get for structured bindings
template<size_t I, typename T, typename Tag>
const T& get(const vec<T, 2, Tag>& v) {
  return v.template get<I>();
}

template<size_t I, typename T, typename Tag>
T& get(vec<T, 2, Tag>& v) {
  return v.template get<I>();
}
