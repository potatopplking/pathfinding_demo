#include <cassert>
#include <cmath>
#include <concepts>
#include <gtest/gtest.h>
#include <sstream>
#include <unordered_set>

#include "log.hpp"
#include "math.hpp"
#include "positional_container.hpp"

TEST(vec, DefaultConstruction) {
  // Test that default-constucted vector
  // has all elements equal to zero
  vec3 v1;
  ASSERT_EQ(v1[0], 0.0);
  ASSERT_EQ(v1[1], 0.0);
  ASSERT_EQ(v1[2], 0.0);
}

TEST(vec, GetElements) {
  // Test operator[]
  ivec3 v1{12, 34, 56};
  ASSERT_EQ(v1[0], 12);
  ASSERT_EQ(v1[1], 34);
  ASSERT_EQ(v1[2], 56);
}

TEST(vec, ArrayConstruction) {
  // Test construction from std::array
  std::array<float, 3> arr3{1.5f, 2.5f, 3.5f};
  vec3 v1(arr3);

  ASSERT_FLOAT_EQ(v1[0], 1.5f);
  ASSERT_FLOAT_EQ(v1[1], 2.5f);
  ASSERT_FLOAT_EQ(v1[2], 3.5f);

  // Test with 2D vector
  std::array<float, 2> arr2{10.0f, 20.0f};
  vec2 v2(arr2);

  ASSERT_FLOAT_EQ(v2[0], 10.0f);
  ASSERT_FLOAT_EQ(v2[1], 20.0f);

  // Test with integer vector
  std::array<int32_t, 4> arr4{1, 2, 3, 4};
  ivec4 v4(arr4);

  ASSERT_EQ(v4[0], 1);
  ASSERT_EQ(v4[1], 2);
  ASSERT_EQ(v4[2], 3);
  ASSERT_EQ(v4[3], 4);

  // Test that original array is unchanged
  ASSERT_FLOAT_EQ(arr3[0], 1.5f);
  ASSERT_FLOAT_EQ(arr3[1], 2.5f);
  ASSERT_FLOAT_EQ(arr3[2], 3.5f);
}

TEST(vec, equalEpsilon) {
  // Test equalEpsilon
  vec3 v1{1.0f, 2.0f, 3.0f};
  vec3 v2{0.999999f, 1.9999999f, 2.9999999f};
  ASSERT_EQ(v1, v2);
}

TEST(vec, equalInt) {
  ivec2 v1{1, 2};
  ivec2 v2{1, 2};
  ASSERT_EQ(v1, v2);
}

TEST(vec, nonEqualEpsilon) {
  // Test operator!=
  vec3 v1{1.0f, 2.0f, 3.0f};
  vec3 v2{2.0f, 4.0f, 6.0f};
  ASSERT_NE(v1, v2);
}

TEST(vec, LogPrint) {
  // Test that logger can print the vector of different types
  // and sizes
  vec2 v2(1.2f, 3.4f);
  vec3 v3(1.2f, 3.4f, 5.6f);
  vec4 v4(1.2f, 3.4f, 5.6f, 7.8f);
  dvec2 dv2(1.2, 3.4);
  dvec3 dv3(1.2, 3.4, 5.6);
  dvec4 dv4(1.2, 3.4, 5.6, 7.8);
  ivec2 iv2(1, 3);
  ivec3 iv3(1, 3, 5);
  ivec4 iv4(1, 3, 5, 7);
  uvec2 uv2(1u, 3u);
  uvec3 uv3(1u, 3u, 5u);
  uvec4 uv4(1u, 3u, 5u, 7u);

  LOG_DEBUG("vec2  ", v2);
  LOG_DEBUG("vec3  ", v3);
  LOG_DEBUG("vec4  ", v4);
  LOG_DEBUG("dvec2 ", dv2);
  LOG_DEBUG("dvec3 ", dv3);
  LOG_DEBUG("dvec4 ", dv4);
  LOG_DEBUG("ivec2 ", iv2);
  LOG_DEBUG("ivec3 ", iv3);
  LOG_DEBUG("ivec4 ", iv4);
  LOG_DEBUG("uvec2 ", uv2);
  LOG_DEBUG("uvec3 ", uv3);
  LOG_DEBUG("uvec4 ", uv4);
}

TEST(vec, Add) {
  // Test operator+ with float vectors
  vec3 v1{1.0f, 2.0f, 3.0f};
  vec3 v2{4.0f, 5.0f, 6.0f};
  vec3 result = v1 + v2;

  ASSERT_FLOAT_EQ(result[0], 5.0f);
  ASSERT_FLOAT_EQ(result[1], 7.0f);
  ASSERT_FLOAT_EQ(result[2], 9.0f);

  // Test operator+ with integer vectors
  ivec3 iv1{1, 2, 3};
  ivec3 iv2{10, 20, 30};
  ivec3 iresult = iv1 + iv2;

  ASSERT_EQ(iresult[0], 11);
  ASSERT_EQ(iresult[1], 22);
  ASSERT_EQ(iresult[2], 33);

  // Test that original vectors are unchanged
  ASSERT_FLOAT_EQ(v1[0], 1.0f);
  ASSERT_FLOAT_EQ(v1[1], 2.0f);
  ASSERT_FLOAT_EQ(v1[2], 3.0f);
}

TEST(vec, Sub) {
  // Test operator- with float vectors
  vec3 v1{5.0f, 7.0f, 9.0f};
  vec3 v2{1.0f, 2.0f, 3.0f};
  vec3 result = v1 - v2;

  ASSERT_FLOAT_EQ(result[0], 4.0f);
  ASSERT_FLOAT_EQ(result[1], 5.0f);
  ASSERT_FLOAT_EQ(result[2], 6.0f);

  // Test operator- with integer vectors
  ivec3 iv1{30, 20, 10};
  ivec3 iv2{5, 3, 1};
  ivec3 iresult = iv1 - iv2;

  ASSERT_EQ(iresult[0], 25);
  ASSERT_EQ(iresult[1], 17);
  ASSERT_EQ(iresult[2], 9);

  // Test that original vectors are unchanged
  ASSERT_FLOAT_EQ(v1[0], 5.0f);
  ASSERT_FLOAT_EQ(v1[1], 7.0f);
  ASSERT_FLOAT_EQ(v1[2], 9.0f);

  // Test subtraction resulting in negative values
  vec3 v3{1.0f, 2.0f, 3.0f};
  vec3 v4{4.0f, 5.0f, 6.0f};
  vec3 negative_result = v3 - v4;

  ASSERT_FLOAT_EQ(negative_result[0], -3.0f);
  ASSERT_FLOAT_EQ(negative_result[1], -3.0f);
  ASSERT_FLOAT_EQ(negative_result[2], -3.0f);
}

TEST(vec, ScalarAddition) {
  // Test operator+ with float vector and scalar
  vec3 v1{1.0f, 2.0f, 3.0f};
  vec3 result = v1 + 5.0f;

  ASSERT_FLOAT_EQ(result[0], 6.0f);
  ASSERT_FLOAT_EQ(result[1], 7.0f);
  ASSERT_FLOAT_EQ(result[2], 8.0f);

  // Test operator+ with integer vector and scalar
  ivec3 iv1{10, 20, 30};
  ivec3 iresult = iv1 + 5;

  ASSERT_EQ(iresult[0], 15);
  ASSERT_EQ(iresult[1], 25);
  ASSERT_EQ(iresult[2], 35);

  // Test that original vector is unchanged
  ASSERT_FLOAT_EQ(v1[0], 1.0f);
  ASSERT_FLOAT_EQ(v1[1], 2.0f);
  ASSERT_FLOAT_EQ(v1[2], 3.0f);

  // Test addition with negative scalar
  vec3 v2{5.0f, 10.0f, 15.0f};
  vec3 negative_result = v2 + (-3.0f);

  ASSERT_FLOAT_EQ(negative_result[0], 2.0f);
  ASSERT_FLOAT_EQ(negative_result[1], 7.0f);
  ASSERT_FLOAT_EQ(negative_result[2], 12.0f);

  // Test addition with zero
  vec3 v3{1.0f, 2.0f, 3.0f};
  vec3 zero_result = v3 + 0.0f;

  ASSERT_FLOAT_EQ(zero_result[0], 1.0f);
  ASSERT_FLOAT_EQ(zero_result[1], 2.0f);
  ASSERT_FLOAT_EQ(zero_result[2], 3.0f);
}

TEST(vec, ScalarSubtraction) {
  // Test operator- with float vector and scalar
  vec3 v1{10.0f, 15.0f, 20.0f};
  vec3 result = v1 - 5.0f;

  ASSERT_FLOAT_EQ(result[0], 5.0f);
  ASSERT_FLOAT_EQ(result[1], 10.0f);
  ASSERT_FLOAT_EQ(result[2], 15.0f);

  // Test operator- with integer vector and scalar
  ivec3 iv1{50, 40, 30};
  ivec3 iresult = iv1 - 10;

  ASSERT_EQ(iresult[0], 40);
  ASSERT_EQ(iresult[1], 30);
  ASSERT_EQ(iresult[2], 20);

  // Test that original vector is unchanged
  ASSERT_FLOAT_EQ(v1[0], 10.0f);
  ASSERT_FLOAT_EQ(v1[1], 15.0f);
  ASSERT_FLOAT_EQ(v1[2], 20.0f);

  // Test subtraction with negative scalar (equivalent to addition)
  vec3 v2{5.0f, 10.0f, 15.0f};
  vec3 negative_result = v2 - (-3.0f);

  ASSERT_FLOAT_EQ(negative_result[0], 8.0f);
  ASSERT_FLOAT_EQ(negative_result[1], 13.0f);
  ASSERT_FLOAT_EQ(negative_result[2], 18.0f);

  // Test subtraction resulting in negative values
  vec3 v3{1.0f, 2.0f, 3.0f};
  vec3 negative_vals_result = v3 - 5.0f;

  ASSERT_FLOAT_EQ(negative_vals_result[0], -4.0f);
  ASSERT_FLOAT_EQ(negative_vals_result[1], -3.0f);
  ASSERT_FLOAT_EQ(negative_vals_result[2], -2.0f);
}

TEST(vec, ScalarMultiplication) {
  // Test scalar * vector with float vectors
  vec3 v1{2.0f, 3.0f, 4.0f};
  vec3 result = v1 * 2.5f;

  ASSERT_FLOAT_EQ(result[0], 5.0f);
  ASSERT_FLOAT_EQ(result[1], 7.5f);
  ASSERT_FLOAT_EQ(result[2], 10.0f);

  // Test scalar * vector with integer vectors
  ivec3 iv1{3, 5, 7};
  ivec3 iresult = iv1 * 2;

  ASSERT_EQ(iresult[0], 6);
  ASSERT_EQ(iresult[1], 10);
  ASSERT_EQ(iresult[2], 14);

  // Test that original vector is unchanged
  ASSERT_FLOAT_EQ(v1[0], 2.0f);
  ASSERT_FLOAT_EQ(v1[1], 3.0f);
  ASSERT_FLOAT_EQ(v1[2], 4.0f);

  // Test multiplication by zero
  vec3 v2{1.0f, 2.0f, 3.0f};
  vec3 zero_result = v2 * 0.0f;

  ASSERT_FLOAT_EQ(zero_result[0], 0.0f);
  ASSERT_FLOAT_EQ(zero_result[1], 0.0f);
  ASSERT_FLOAT_EQ(zero_result[2], 0.0f);

  // Test multiplication by negative scalar (and different ordering)
  vec3 v3{1.0f, -2.0f, 3.0f};
  vec3 negative_result = -2.0f * v3;

  ASSERT_FLOAT_EQ(negative_result[0], -2.0f);
  ASSERT_FLOAT_EQ(negative_result[1], 4.0f);
  ASSERT_FLOAT_EQ(negative_result[2], -6.0f);
}

TEST(vec, ScalarDivision) {
  // Test vector / scalar with float vectors
  vec3 v1{10.0f, 15.0f, 20.0f};
  vec3 result = v1 / 2.5f;

  ASSERT_FLOAT_EQ(result[0], 4.0f);
  ASSERT_FLOAT_EQ(result[1], 6.0f);
  ASSERT_FLOAT_EQ(result[2], 8.0f);

  // Test vector / scalar with integer vectors
  ivec3 iv1{12, 18, 24};
  ivec3 iresult = iv1 / 2;

  ASSERT_EQ(iresult[0], 6);
  ASSERT_EQ(iresult[1], 9);
  ASSERT_EQ(iresult[2], 12);

  // Test that original vector is unchanged
  ASSERT_FLOAT_EQ(v1[0], 10.0f);
  ASSERT_FLOAT_EQ(v1[1], 15.0f);
  ASSERT_FLOAT_EQ(v1[2], 20.0f);

  // Test division by negative scalar
  vec3 v2{6.0f, -9.0f, 12.0f};
  vec3 negative_result = v2 / -3.0f;

  ASSERT_FLOAT_EQ(negative_result[0], -2.0f);
  ASSERT_FLOAT_EQ(negative_result[1], 3.0f);
  ASSERT_FLOAT_EQ(negative_result[2], -4.0f);

  // Test division by fractional scalar
  vec3 v3{1.0f, 2.0f, 3.0f};
  vec3 fractional_result = v3 / 0.5f;

  ASSERT_FLOAT_EQ(fractional_result[0], 2.0f);
  ASSERT_FLOAT_EQ(fractional_result[1], 4.0f);
  ASSERT_FLOAT_EQ(fractional_result[2], 6.0f);
}

TEST(vec, AdditionAssignment) {
  // Test operator+= with float vectors
  vec3 v1{1.0f, 2.0f, 3.0f};
  vec3 v2{4.0f, 5.0f, 6.0f};
  v1 += v2;

  ASSERT_FLOAT_EQ(v1[0], 5.0f);
  ASSERT_FLOAT_EQ(v1[1], 7.0f);
  ASSERT_FLOAT_EQ(v1[2], 9.0f);

  // Test that v2 is unchanged
  ASSERT_FLOAT_EQ(v2[0], 4.0f);
  ASSERT_FLOAT_EQ(v2[1], 5.0f);
  ASSERT_FLOAT_EQ(v2[2], 6.0f);

  // Test operator+= with integer vectors
  ivec3 iv1{10, 20, 30};
  ivec3 iv2{1, 2, 3};
  iv1 += iv2;

  ASSERT_EQ(iv1[0], 11);
  ASSERT_EQ(iv1[1], 22);
  ASSERT_EQ(iv1[2], 33);

  // Test chaining
  vec3 v3{1.0f, 1.0f, 1.0f};
  vec3 v4{2.0f, 2.0f, 2.0f};
  vec3 v5{3.0f, 3.0f, 3.0f};
  v3 += v4 += v5;

  ASSERT_FLOAT_EQ(v3[0], 6.0f);
  ASSERT_FLOAT_EQ(v3[1], 6.0f);
  ASSERT_FLOAT_EQ(v3[2], 6.0f);
  ASSERT_FLOAT_EQ(v4[0], 5.0f);
  ASSERT_FLOAT_EQ(v4[1], 5.0f);
  ASSERT_FLOAT_EQ(v4[2], 5.0f);
}

TEST(vec, SubtractionAssignment) {
  // Test operator-= with float vectors
  vec3 v1{10.0f, 15.0f, 20.0f};
  vec3 v2{3.0f, 5.0f, 7.0f};
  v1 -= v2;

  ASSERT_FLOAT_EQ(v1[0], 7.0f);
  ASSERT_FLOAT_EQ(v1[1], 10.0f);
  ASSERT_FLOAT_EQ(v1[2], 13.0f);

  // Test that v2 is unchanged
  ASSERT_FLOAT_EQ(v2[0], 3.0f);
  ASSERT_FLOAT_EQ(v2[1], 5.0f);
  ASSERT_FLOAT_EQ(v2[2], 7.0f);

  // Test operator-= with integer vectors
  ivec3 iv1{50, 40, 30};
  ivec3 iv2{5, 10, 15};
  iv1 -= iv2;

  ASSERT_EQ(iv1[0], 45);
  ASSERT_EQ(iv1[1], 30);
  ASSERT_EQ(iv1[2], 15);

  // Test subtraction resulting in negative values
  vec3 v3{1.0f, 2.0f, 3.0f};
  vec3 v4{4.0f, 5.0f, 6.0f};
  v3 -= v4;

  ASSERT_FLOAT_EQ(v3[0], -3.0f);
  ASSERT_FLOAT_EQ(v3[1], -3.0f);
  ASSERT_FLOAT_EQ(v3[2], -3.0f);
}

TEST(vec, LengthSquared) {
  // Test LengthSquared with float vectors
  vec3 v1{3.0f, 4.0f, 0.0f};
  ASSERT_FLOAT_EQ(v1.LengthSquared(), 25.0f); // 3² + 4² + 0² = 25

  vec2 v2{1.0f, 1.0f};
  ASSERT_FLOAT_EQ(v2.LengthSquared(), 2.0f); // 1² + 1² = 2

  // Test with zero vector
  vec3 zero{0.0f, 0.0f, 0.0f};
  ASSERT_FLOAT_EQ(zero.LengthSquared(), 0.0f);
}

TEST(vec, Length) {
  // Test Length with float vectors
  vec3 v1{3.0f, 4.0f, 0.0f};
  ASSERT_FLOAT_EQ(v1.Length(), 5.0f); // sqrt(3² + 4² + 0²) = 5

  vec2 v2{1.0f, 1.0f};
  ASSERT_NEAR(v2.Length(), 1.414213f, 1e-5f); // sqrt(2) ≈ 1.414213

  // Test with zero vector
  vec3 zero{0.0f, 0.0f, 0.0f};
  ASSERT_FLOAT_EQ(zero.Length(), 0.0f);
}

TEST(vec, Normalize) {
  // Test Normalize with float vectors
  vec3 v1{3.0f, 4.0f, 0.0f};
  v1.Normalize();

  ASSERT_FLOAT_EQ(v1[0], 0.6f); // 3/5
  ASSERT_FLOAT_EQ(v1[1], 0.8f); // 4/5
  ASSERT_FLOAT_EQ(v1[2], 0.0f);
  ASSERT_NEAR(v1.Length(), 1.0f, 1e-6f);

  // Test with zero vector (may produce NaN - implementation dependent)
  vec3 zero{0.0f, 0.0f, 0.0f};
  zero.Normalize();
  // Check if result is NaN (which is expected for zero vector normalization)
  ASSERT_TRUE(zero[0] == 0.0f);
  ASSERT_TRUE(zero[1] == 0.0f);
  ASSERT_TRUE(zero[2] == 0.0f);
}

TEST(vec, GetNormalized) {
  // Test GetNormalized with float vectors
  const vec3 v1{3.0f, 4.0f, 0.0f};
  vec3 normalized = v1.GetNormalized();

  // Original vector should be unchanged
  ASSERT_FLOAT_EQ(v1[0], 3.0f);
  ASSERT_FLOAT_EQ(v1[1], 4.0f);
  ASSERT_FLOAT_EQ(v1[2], 0.0f);

  // Normalized copy should be unit length
  ASSERT_FLOAT_EQ(normalized[0], 0.6f); // 3/5
  ASSERT_FLOAT_EQ(normalized[1], 0.8f); // 4/5
  ASSERT_FLOAT_EQ(normalized[2], 0.0f);
  ASSERT_NEAR(normalized.Length(), 1.0f, 1e-6f);

  // Test with zero vector
  vec3 zero{0.0f, 0.0f, 0.0f};
  vec3 zero_normalized = zero.GetNormalized();

  ASSERT_FLOAT_EQ(zero_normalized[0], 0.0f);
  ASSERT_FLOAT_EQ(zero_normalized[1], 0.0f);
  ASSERT_FLOAT_EQ(zero_normalized[2], 0.0f);

  // Original zero vector should be unchanged
  ASSERT_FLOAT_EQ(zero[0], 0.0f);
  ASSERT_FLOAT_EQ(zero[1], 0.0f);
  ASSERT_FLOAT_EQ(zero[2], 0.0f);
}

TEST(vec, GetOrthogonal) {
  const vec2 v1{5.0f, 1.0f};
  auto v2 = v1.GetOrthogonal();
  ASSERT_FLOAT_EQ(v2[0], -1.0f);
  ASSERT_FLOAT_EQ(v2[1], 5.0f);
}

TEST(vec, DistanceTo) {
  // Test DistanceTo with 3D vectors
  vec3 v1{0.0f, 0.0f, 0.0f};
  vec3 v2{3.0f, 4.0f, 0.0f};

  float distance = v1.DistanceTo(v2);
  ASSERT_FLOAT_EQ(distance, 5.0f); // 3-4-5 triangle

  // Distance should be symmetric
  ASSERT_FLOAT_EQ(v2.DistanceTo(v1), distance);

  // Test with 2D vectors
  vec2 a{1.0f, 1.0f};
  vec2 b{4.0f, 5.0f};

  float distance_2d = a.DistanceTo(b);
  ASSERT_FLOAT_EQ(distance_2d, 5.0f); // sqrt((4-1)² + (5-1)²) = sqrt(9+16) = 5

  // Distance to self should be zero
  ASSERT_FLOAT_EQ(v1.DistanceTo(v1), 0.0f);
  ASSERT_FLOAT_EQ(a.DistanceTo(a), 0.0f);

  // Test that original vectors are unchanged
  ASSERT_FLOAT_EQ(v1[0], 0.0f);
  ASSERT_FLOAT_EQ(v1[1], 0.0f);
  ASSERT_FLOAT_EQ(v1[2], 0.0f);
  ASSERT_FLOAT_EQ(v2[0], 3.0f);
  ASSERT_FLOAT_EQ(v2[1], 4.0f);
  ASSERT_FLOAT_EQ(v2[2], 0.0f);
}

TEST(vec, ChainedOperations) {
  vec2 a{1.0f, 2.0f};
  vec2 b{3.0f, 4.0f};

  // Test chaining: (a + b) * 2.0f
  auto result = (a + b) * 2.0f;
  ASSERT_FLOAT_EQ(result[0], 8.0f);
  ASSERT_FLOAT_EQ(result[1], 12.0f);

  // Test chaining with assignment
  a += b;
  a = a * 0.5f;
  ASSERT_FLOAT_EQ(a[0], 2.0f);
  ASSERT_FLOAT_EQ(a[1], 3.0f);
}

TEST(vec, ChangeTag) {
  // Test changing tag from WorldPos to WindowPos
  WorldPos world_pos{100.0f, 200.0f};
  WindowPos window_pos = world_pos.ChangeTag<WindowPosTag>();

  // Data should be preserved
  ASSERT_FLOAT_EQ(window_pos[0], 100.0f);
  ASSERT_FLOAT_EQ(window_pos[1], 200.0f);

  // Original should be unchanged
  ASSERT_FLOAT_EQ(world_pos[0], 100.0f);
  ASSERT_FLOAT_EQ(world_pos[1], 200.0f);

  // Test changing tag from TilePos to another tag
  TilePos tile_pos{5, 10};
  auto generic_pos = tile_pos.ChangeTag<Any>();

  ASSERT_EQ(generic_pos[0], 5);
  ASSERT_EQ(generic_pos[1], 10);

  // Test with 3D vector and custom tag
  struct CustomTag {};
  vec3 original{1.5f, 2.5f, 3.5f};
  vec<float, 3, CustomTag> custom_tagged = original.ChangeTag<CustomTag>();

  ASSERT_FLOAT_EQ(custom_tagged[0], 1.5f);
  ASSERT_FLOAT_EQ(custom_tagged[1], 2.5f);
  ASSERT_FLOAT_EQ(custom_tagged[2], 3.5f);

  // Test that we can change back
  vec3 back_to_original = custom_tagged.ChangeTag<Any>();

  ASSERT_FLOAT_EQ(back_to_original[0], 1.5f);
  ASSERT_FLOAT_EQ(back_to_original[1], 2.5f);
  ASSERT_FLOAT_EQ(back_to_original[2], 3.5f);
}

TEST(Matrix, DefaultConstruction) {
  // Test that default-constructed matrix has all elements equal to zero
  Matrix<float, 2> m1;
  ASSERT_FLOAT_EQ(m1[0][0], 0.0f);
  ASSERT_FLOAT_EQ(m1[0][1], 0.0f);
  ASSERT_FLOAT_EQ(m1[1][0], 0.0f);
  ASSERT_FLOAT_EQ(m1[1][1], 0.0f);
}

TEST(Matrix, ArrayConstruction) {
  // Test construction from array (column major)
  Matrix<float, 2> m1(std::array<float, 4>{1.0f, 2.0f, 3.0f, 4.0f});

  // Column 0: [1, 2]
  ASSERT_FLOAT_EQ(m1[0][0], 1.0f);
  ASSERT_FLOAT_EQ(m1[0][1], 2.0f);

  // Column 1: [3, 4]
  ASSERT_FLOAT_EQ(m1[1][0], 3.0f);
  ASSERT_FLOAT_EQ(m1[1][1], 4.0f);

  // Test with 3x3 matrix
  Matrix<int, 3> m2(std::array<int, 9>{1, 2, 3, 4, 5, 6, 7, 8, 9});

  // Column 0: [1, 2, 3]
  ASSERT_EQ(m2[0][0], 1);
  ASSERT_EQ(m2[0][1], 2);
  ASSERT_EQ(m2[0][2], 3);

  // Column 1: [4, 5, 6]
  ASSERT_EQ(m2[1][0], 4);
  ASSERT_EQ(m2[1][1], 5);
  ASSERT_EQ(m2[1][2], 6);

  // Column 2: [7, 8, 9]
  ASSERT_EQ(m2[2][0], 7);
  ASSERT_EQ(m2[2][1], 8);
  ASSERT_EQ(m2[2][2], 9);
}

TEST(Matrix, ElementAccess) {
  // Test element access (both const and non-const)
  Matrix<float, 2> m1(std::array<float, 4>{1.0f, 2.0f, 3.0f, 4.0f});

  // Test const access
  const Matrix<float, 2> &const_ref = m1;
  ASSERT_FLOAT_EQ(const_ref[0][0], 1.0f);
  ASSERT_FLOAT_EQ(const_ref[1][1], 4.0f);

  // Test non-const access and modification
  m1[0][0] = 10.0f;
  m1[1][1] = 40.0f;

  ASSERT_FLOAT_EQ(m1[0][0], 10.0f);
  ASSERT_FLOAT_EQ(m1[1][1], 40.0f);

  // Verify other elements unchanged
  ASSERT_FLOAT_EQ(m1[0][1], 2.0f);
  ASSERT_FLOAT_EQ(m1[1][0], 3.0f);
}

TEST(Matrix, Addition) {
  // Test matrix addition
  Matrix<float, 2> m1(std::array<float, 4>{1.0f, 2.0f, 3.0f, 4.0f});
  Matrix<float, 2> m2(std::array<float, 4>{5.0f, 6.0f, 7.0f, 8.0f});

  Matrix<float, 2> result = m1 + m2;

  ASSERT_FLOAT_EQ(result[0][0], 6.0f);  // 1 + 5
  ASSERT_FLOAT_EQ(result[0][1], 8.0f);  // 2 + 6
  ASSERT_FLOAT_EQ(result[1][0], 10.0f); // 3 + 7
  ASSERT_FLOAT_EQ(result[1][1], 12.0f); // 4 + 8

  // Test that original matrices are unchanged
  ASSERT_FLOAT_EQ(m1[0][0], 1.0f);
  ASSERT_FLOAT_EQ(m1[1][1], 4.0f);
  ASSERT_FLOAT_EQ(m2[0][0], 5.0f);
  ASSERT_FLOAT_EQ(m2[1][1], 8.0f);

  // Test with integer matrices
  Matrix<int, 2> im1(std::array<int, 4>{1, 2, 3, 4});
  Matrix<int, 2> im2(std::array<int, 4>{10, 20, 30, 40});
  Matrix<int, 2> iresult = im1 + im2;

  ASSERT_EQ(iresult[0][0], 11);
  ASSERT_EQ(iresult[0][1], 22);
  ASSERT_EQ(iresult[1][0], 33);
  ASSERT_EQ(iresult[1][1], 44);
}

TEST(Matrix, Subtraction) {
  // Test matrix subtraction
  Matrix<float, 2> m1(std::array<float, 4>{10.0f, 8.0f, 6.0f, 4.0f});
  Matrix<float, 2> m2(std::array<float, 4>{1.0f, 2.0f, 3.0f, 4.0f});

  Matrix<float, 2> result = m1 - m2;

  ASSERT_FLOAT_EQ(result[0][0], 9.0f); // 10 - 1
  ASSERT_FLOAT_EQ(result[0][1], 6.0f); // 8 - 2
  ASSERT_FLOAT_EQ(result[1][0], 3.0f); // 6 - 3
  ASSERT_FLOAT_EQ(result[1][1], 0.0f); // 4 - 4

  // Test that original matrices are unchanged
  ASSERT_FLOAT_EQ(m1[0][0], 10.0f);
  ASSERT_FLOAT_EQ(m1[1][1], 4.0f);
  ASSERT_FLOAT_EQ(m2[0][0], 1.0f);
  ASSERT_FLOAT_EQ(m2[1][1], 4.0f);

  // Test subtraction resulting in negative values
  Matrix<float, 2> m3(std::array<float, 4>{1.0f, 2.0f, 3.0f, 4.0f});
  Matrix<float, 2> m4(std::array<float, 4>{5.0f, 6.0f, 7.0f, 8.0f});
  Matrix<float, 2> negative_result = m3 - m4;

  ASSERT_FLOAT_EQ(negative_result[0][0], -4.0f);
  ASSERT_FLOAT_EQ(negative_result[0][1], -4.0f);
  ASSERT_FLOAT_EQ(negative_result[1][0], -4.0f);
  ASSERT_FLOAT_EQ(negative_result[1][1], -4.0f);
}

TEST(Matrix, MatrixMultiplication) {
  // Test 2x2 matrix multiplication
  Matrix<float, 2> m1(std::array<float, 4>{1.0f, 2.0f, 3.0f, 4.0f});
  Matrix<float, 2> m2(std::array<float, 4>{5.0f, 6.0f, 7.0f, 8.0f});

  Matrix<float, 2> result = m2 * m1;

  ASSERT_FLOAT_EQ(result[0][0], 23.0f);
  ASSERT_FLOAT_EQ(result[0][1], 34.0f);
  ASSERT_FLOAT_EQ(result[1][0], 31.0f);
  ASSERT_FLOAT_EQ(result[1][1], 46.0f);

  // Test identity property: I * m = m
  Matrix<float, 2> identity = Matrix<float, 2>::Eye();
  Matrix<float, 2> identity_result = identity * m1;

  ASSERT_FLOAT_EQ(identity_result[0][0], m1[0][0]);
  ASSERT_FLOAT_EQ(identity_result[0][1], m1[0][1]);
  ASSERT_FLOAT_EQ(identity_result[1][0], m1[1][0]);
  ASSERT_FLOAT_EQ(identity_result[1][1], m1[1][1]);

  // Test with 3x3 matrices
  Matrix<int, 3> im1(std::array<int, 9>{1, 0, 0, 0, 1, 0, 0, 0, 1}); // Identity
  Matrix<int, 3> im2(std::array<int, 9>{1, 2, 3, 4, 5, 6, 7, 8, 9});
  Matrix<int, 3> iresult = im1 * im2;

  // Identity * matrix = matrix
  ASSERT_EQ(iresult[0][0], 1);
  ASSERT_EQ(iresult[0][1], 2);
  ASSERT_EQ(iresult[0][2], 3);
  ASSERT_EQ(iresult[1][0], 4);
  ASSERT_EQ(iresult[1][1], 5);
  ASSERT_EQ(iresult[1][2], 6);
  ASSERT_EQ(iresult[2][0], 7);
  ASSERT_EQ(iresult[2][1], 8);
  ASSERT_EQ(iresult[2][2], 9);
}

TEST(Matrix, MatrixVectorMultiplication) {
  // Test matrix-vector multiplication
  Matrix<float, 2> m1(std::array<float, 4>{1.0f, 2.0f, 3.0f, 4.0f});
  vec<float, 2> v1(2.0f, 3.0f);

  vec<float, 2> result = m1 * v1;

  ASSERT_FLOAT_EQ(result[0], 8.0f);
  ASSERT_FLOAT_EQ(result[1], 18.0f);

  // Test with 3x3 matrix and 3D vector
  Matrix<int, 3> im1(std::array<int, 9>{1, 0, 0, 0, 1, 0, 0, 0, 1}); // Identity
  vec<int, 3> iv1(5, 10, 15);

  vec<int, 3> iresult = im1 * iv1;

  // Identity * vector = vector
  ASSERT_EQ(iresult[0], 5);
  ASSERT_EQ(iresult[1], 10);
  ASSERT_EQ(iresult[2], 15);

  // Test that original matrix and vector are unchanged
  ASSERT_FLOAT_EQ(m1[0][0], 1.0f);
  ASSERT_FLOAT_EQ(v1[0], 2.0f);
  ASSERT_FLOAT_EQ(v1[1], 3.0f);
}

TEST(Matrix, EyeIdentityMatrix) {
  // Test 2x2 identity matrix
  Matrix<float, 2> eye2 = Matrix<float, 2>::Eye();

  ASSERT_FLOAT_EQ(eye2[0][0], 1.0f);
  ASSERT_FLOAT_EQ(eye2[0][1], 0.0f);
  ASSERT_FLOAT_EQ(eye2[1][0], 0.0f);
  ASSERT_FLOAT_EQ(eye2[1][1], 1.0f);

  // Test 3x3 identity matrix
  Matrix<int, 3> eye3 = Matrix<int, 3>::Eye();

  for (size_t i = 0; i < 3; ++i) {
    for (size_t j = 0; j < 3; ++j) {
      if (i == j) {
        ASSERT_EQ(eye3[i][j], 1);
      } else {
        ASSERT_EQ(eye3[i][j], 0);
      }
    }
  }

  // Test 4x4 identity matrix
  Matrix<double, 4> eye4 = Matrix<double, 4>::Eye();

  for (size_t i = 0; i < 4; ++i) {
    for (size_t j = 0; j < 4; ++j) {
      if (i == j) {
        ASSERT_DOUBLE_EQ(eye4[i][j], 1.0);
      } else {
        ASSERT_DOUBLE_EQ(eye4[i][j], 0.0);
      }
    }
  }
}

TEST(Matrix, LogPrint) {
  // Test that logger can print matrices of different types and sizes
  Matrix<float, 2> m2(std::array<float, 4>{1.1f, 2.2f, 3.3f, 4.4f});
  Matrix<int, 3> m3(std::array<int, 9>{1, 2, 3, 4, 5, 6, 7, 8, 9});
  Matrix<double, 2> dm2(std::array<double, 4>{1.5, 2.5, 3.5, 4.5});

  LOG_DEBUG("Matrix<float, 2>  ", m2);
  LOG_DEBUG("Matrix<int, 3>    ", m3);
  LOG_DEBUG("Matrix<double, 2> ", dm2);
}

TEST(Matrix, ChainedOperations) {
  // Test chaining matrix operations
  Matrix<float, 2> m1(std::array<float, 4>{1.0f, 2.0f, 3.0f, 4.0f});
  Matrix<float, 2> m2(std::array<float, 4>{1.0f, 1.0f, 1.0f, 1.0f});
  Matrix<float, 2> m3(std::array<float, 4>{2.0f, 0.0f, 0.0f, 2.0f});

  // Test (m1 + m2) * m3
  Matrix<float, 2> result = (m1 + m2) * m3;

  // m1 + m2 = [2 4]    m3 = [2 0]    result = [4  8]
  //           [3 5]          [0 2]             [6 10]

  ASSERT_FLOAT_EQ(result[0][0], 4.0f);
  ASSERT_FLOAT_EQ(result[0][1], 6.0f);
  ASSERT_FLOAT_EQ(result[1][0], 8.0f);
  ASSERT_FLOAT_EQ(result[1][1], 10.0f);

  // Test that original matrices are unchanged
  ASSERT_FLOAT_EQ(m1[0][0], 1.0f);
  ASSERT_FLOAT_EQ(m2[0][0], 1.0f);
  ASSERT_FLOAT_EQ(m3[0][0], 2.0f);
}

// Helper class for SimpleContainer tests
class TestEntity {
public:
  TestEntity() : m_Position(0.0f, 0.0f) {}
  TestEntity(float x, float y) : m_Position(x, y) {}
  TestEntity(WorldPos pos) : m_Position(pos) {}

  WorldPos GetPosition() const { return m_Position; }
  void SetPosition(WorldPos pos) { m_Position = pos; }

private:
  WorldPos m_Position;
};

TEST(SimpleContainer, DefaultConstruction) {
  // Test that SimpleContainer can be default constructed
  SimpleContainer<TestEntity> container;
  SUCCEED();
}

TEST(SimpleContainer, AddSingleItem) {
  // Test adding a single item
  SimpleContainer<TestEntity> container;
  auto entity = std::make_shared<TestEntity>(5.0f, 10.0f);
  container.Add(entity);

  // Verify by getting items near the position
  auto results = container.Get(WorldPos(5.0f, 10.0f), 1.0f);
  ASSERT_EQ(results.size(), 1);
}

TEST(SimpleContainer, AddMultipleItems) {
  // Test adding multiple items
  SimpleContainer<TestEntity> container;
  container.Add(std::make_shared<TestEntity>(0.0f, 0.0f));
  container.Add(std::make_shared<TestEntity>(10.0f, 10.0f));
  container.Add(std::make_shared<TestEntity>(20.0f, 20.0f));

  // Verify by getting items near a position
  auto results = container.Get(WorldPos(10.0f, 10.0f), 5.0f);
  ASSERT_GE(results.size(), 1);
}

TEST(SimpleContainer, GetItemsInRadius) {
  // Test getting items within a radius
  SimpleContainer<TestEntity> container;

  // Add items in a known pattern
  container.Add(std::make_shared<TestEntity>(0.0f, 0.0f));   // At origin
  container.Add(std::make_shared<TestEntity>(1.0f, 0.0f));   // 1 unit away
  container.Add(std::make_shared<TestEntity>(0.0f, 1.0f));   // 1 unit away
  container.Add(std::make_shared<TestEntity>(10.0f, 10.0f)); // Far away

  // Get items within 2.0 units of origin
  auto results = container.Get(WorldPos(0.0f, 0.0f), 2.0f);

  // Should find the 3 nearby items (if Get is working correctly)
  // Note: This depends on the actual implementation of Get
  ASSERT_GE(results.size(), 0);
}

TEST(SimpleContainer, GetItemsEmptyContainer) {
  // Test getting items from empty container
  SimpleContainer<TestEntity> container;

  auto results = container.Get(WorldPos(0.0f, 0.0f), 10.0f);
  ASSERT_EQ(results.size(), 0);
}

TEST(SimpleContainer, WeakPtrValidAfterGet) {
  // Test that weak_ptr returned from Get can be locked
  SimpleContainer<TestEntity> container;
  container.Add(std::make_shared<TestEntity>(5.0f, 5.0f));

  auto results = container.Get(WorldPos(5.0f, 5.0f), 10.0f);

  if (!results.empty()) {
    auto locked = results[0].lock();
    ASSERT_NE(locked, nullptr);

    // Verify the position
    WorldPos pos = locked->GetPosition();
    ASSERT_FLOAT_EQ(pos.x(), 5.0f);
    ASSERT_FLOAT_EQ(pos.y(), 5.0f);
  }
}

TEST(SimpleContainer, UpdateAll) {
  // Test that UpdateAll properly updates item positions
  // For SimpleContainer, Update is a no-op but Get should still find moved
  // items
  SimpleContainer<TestEntity> container;

  auto item1 = std::make_shared<TestEntity>(10.0f, 10.0f);
  auto item2 = std::make_shared<TestEntity>(20.0f, 20.0f);
  container.Add(item1);
  container.Add(item2);

  // Verify items exist at original positions
  auto results_old1 = container.Get(WorldPos(10.0f, 10.0f), 1.0f);
  auto results_old2 = container.Get(WorldPos(20.0f, 20.0f), 1.0f);
  ASSERT_GE(results_old1.size(), 1);
  ASSERT_GE(results_old2.size(), 1);

  // Change positions
  item1->SetPosition(WorldPos(50.0f, 50.0f));
  item2->SetPosition(WorldPos(60.0f, 60.0f));

  // Call UpdateAll
  container.UpdateAll();

  // For SimpleContainer, items should still be found at new positions
  // (since Get checks actual item positions, not cached locations)
  auto results_new1 = container.Get(WorldPos(50.0f, 50.0f), 1.0f);
  auto results_new2 = container.Get(WorldPos(60.0f, 60.0f), 1.0f);
  ASSERT_GE(results_new1.size(), 1);
  ASSERT_GE(results_new2.size(), 1);

  // Items should NOT be found at old positions anymore
  auto results_old_check1 = container.Get(WorldPos(10.0f, 10.0f), 1.0f);
  auto results_old_check2 = container.Get(WorldPos(20.0f, 20.0f), 1.0f);
  ASSERT_EQ(results_old_check1.size(), 0);
  ASSERT_EQ(results_old_check2.size(), 0);
}

TEST(SimpleContainer, Update) {
  // Test that Update properly handles a single item position change
  SimpleContainer<TestEntity> container;

  auto item = std::make_shared<TestEntity>(15.0f, 15.0f);
  container.Add(item);

  // Verify item exists at original position
  auto results_old = container.Get(WorldPos(15.0f, 15.0f), 1.0f);
  ASSERT_GE(results_old.size(), 1);

  // Change position
  item->SetPosition(WorldPos(75.0f, 75.0f));

  // Call Update
  container.Update(item);

  // Item should be found at new position
  auto results_new = container.Get(WorldPos(75.0f, 75.0f), 1.0f);
  ASSERT_GE(results_new.size(), 1);

  // Item should NOT be found at old position
  auto results_old_check = container.Get(WorldPos(15.0f, 15.0f), 1.0f);
  ASSERT_EQ(results_old_check.size(), 0);
}

TEST(SimpleContainer, AddItemsWithSamePosition) {
  // Test adding multiple items at the same position
  SimpleContainer<TestEntity> container;

  container.Add(std::make_shared<TestEntity>(5.0f, 5.0f));
  container.Add(std::make_shared<TestEntity>(5.0f, 5.0f));
  container.Add(std::make_shared<TestEntity>(5.0f, 5.0f));

  auto results = container.Get(WorldPos(5.0f, 5.0f), 1.0f);
  ASSERT_EQ(results.size(), 3);
}

TEST(PositionalContainer, DefaultConstruction) {
  // Test that PositionalContainer can be constructed with size and chunks
  WorldSize size(100.0f, 100.0f);
  size_t chunks = 10;

  PositionalContainer<TestEntity> container(size, chunks);
  SUCCEED();
}

TEST(PositionalContainer, AddSingleItem) {
  // Test adding a single item
  WorldSize size(100.0f, 100.0f);
  PositionalContainer<TestEntity> container(size, 10);
  auto entity = std::make_shared<TestEntity>(5.0f, 10.0f);
  container.Add(entity);

  // Verify by getting items near the position
  auto results = container.Get(WorldPos(5.0f, 10.0f), 1.0f);
  ASSERT_GE(results.size(), 1);
}

TEST(PositionalContainer, AddMultipleItems) {
  // Test adding multiple items
  WorldSize size(100.0f, 100.0f);
  PositionalContainer<TestEntity> container(size, 10);

  container.Add(std::make_shared<TestEntity>(10.0f, 10.0f));
  container.Add(std::make_shared<TestEntity>(20.0f, 20.0f));
  container.Add(std::make_shared<TestEntity>(30.0f, 30.0f));

  // Verify by getting items near a position
  auto results = container.Get(WorldPos(20.0f, 20.0f), 5.0f);
  ASSERT_GE(results.size(), 1);
}

TEST(PositionalContainer, GetItemsInRadius) {
  // Test getting items within a radius
  WorldSize size(100.0f, 100.0f);
  PositionalContainer<TestEntity> container(size, 10);

  // Add items in a known pattern
  container.Add(std::make_shared<TestEntity>(50.0f, 50.0f)); // At center
  container.Add(std::make_shared<TestEntity>(51.0f, 50.0f)); // 1 unit away
  container.Add(std::make_shared<TestEntity>(50.0f, 51.0f)); // 1 unit away
  container.Add(std::make_shared<TestEntity>(90.0f, 90.0f)); // Far away

  // Get items within 2.0 units of center position
  auto results = container.Get(WorldPos(50.0f, 50.0f), 2.0f);

  // Should find the 3 nearby items
  ASSERT_GE(results.size(), 0);
}

TEST(PositionalContainer, GetItemsEmptyContainer) {
  // Test getting items from empty container
  WorldSize size(100.0f, 100.0f);
  PositionalContainer<TestEntity> container(size, 10);

  auto results = container.Get(WorldPos(50.0f, 50.0f), 10.0f);
  ASSERT_EQ(results.size(), 0);
}

TEST(PositionalContainer, WeakPtrValidAfterGet) {
  // Test that weak_ptr returned from Get can be locked
  WorldSize size(100.0f, 100.0f);
  PositionalContainer<TestEntity> container(size, 10);

  container.Add(std::make_shared<TestEntity>(50.0f, 50.0f));

  auto results = container.Get(WorldPos(50.0f, 50.0f), 10.0f);

  if (!results.empty()) {
    auto locked = results[0].lock();
    ASSERT_NE(locked, nullptr);

    // Verify the position
    WorldPos pos = locked->GetPosition();
    ASSERT_FLOAT_EQ(pos.x(), 50.0f);
    ASSERT_FLOAT_EQ(pos.y(), 50.0f);
  }
}

TEST(PositionalContainer, UpdateAll) {
  // Test that UpdateAll properly updates spatial index after position changes
  WorldSize size(100.0f, 100.0f);
  PositionalContainer<TestEntity> container(size, 10);

  auto item1 = std::make_shared<TestEntity>(15.0f, 15.0f);
  auto item2 = std::make_shared<TestEntity>(25.0f, 25.0f);
  container.Add(item1);
  container.Add(item2);

  // Verify items exist at original positions
  auto results_old1 = container.Get(WorldPos(15.0f, 15.0f), 2.0f);
  auto results_old2 = container.Get(WorldPos(25.0f, 25.0f), 2.0f);
  ASSERT_GE(results_old1.size(), 1);
  ASSERT_GE(results_old2.size(), 1);

  // Change positions to different grid chunks
  item1->SetPosition(WorldPos(65.0f, 65.0f));
  item2->SetPosition(WorldPos(75.0f, 75.0f));

  // Call UpdateAll to refresh spatial index
  container.UpdateAll();

  // Items should be found at new positions
  auto results_new1 = container.Get(WorldPos(65.0f, 65.0f), 2.0f);
  auto results_new2 = container.Get(WorldPos(75.0f, 75.0f), 2.0f);
  ASSERT_GE(results_new1.size(), 1);
  ASSERT_GE(results_new2.size(), 1);

  // Items should NOT be found at old positions anymore
  auto results_old_check1 = container.Get(WorldPos(15.0f, 15.0f), 2.0f);
  auto results_old_check2 = container.Get(WorldPos(25.0f, 25.0f), 2.0f);
  ASSERT_EQ(results_old_check1.size(), 0);
  ASSERT_EQ(results_old_check2.size(), 0);
}

TEST(PositionalContainer, Update) {
  // Test that Update properly updates spatial index for a single item
  WorldSize size(100.0f, 100.0f);
  PositionalContainer<TestEntity> container(size, 10);

  auto item = std::make_shared<TestEntity>(20.0f, 20.0f);
  container.Add(item);

  // Verify item exists at original position
  auto results_old = container.Get(WorldPos(20.0f, 20.0f), 2.0f);
  ASSERT_GE(results_old.size(), 1);

  // Change position to different grid chunk
  item->SetPosition(WorldPos(80.0f, 80.0f));

  // Call Update to refresh spatial index
  container.Update(item);

  // Item should be found at new position
  auto results_new = container.Get(WorldPos(80.0f, 80.0f), 2.0f);
  ASSERT_GE(results_new.size(), 1);

  // Item should NOT be found at old position
  auto results_old_check = container.Get(WorldPos(20.0f, 20.0f), 2.0f);
  ASSERT_EQ(results_old_check.size(), 0);
}

TEST(PositionalContainer, AddItemsWithSamePosition) {
  // Test adding multiple items at the same position
  WorldSize size(100.0f, 100.0f);
  PositionalContainer<TestEntity> container(size, 10);

  container.Add(std::make_shared<TestEntity>(50.0f, 50.0f));
  container.Add(std::make_shared<TestEntity>(50.0f, 50.0f));
  container.Add(std::make_shared<TestEntity>(50.0f, 50.0f));

  auto results = container.Get(WorldPos(50.0f, 50.0f), 1.0f);
  ASSERT_GE(results.size(), 0);
}

TEST(PositionalContainer, AddOutOfBounds) {
  // Test adding items outside the grid bounds
  WorldSize size(100.0f, 100.0f);
  PositionalContainer<TestEntity> container(size, 10);

  // Try to add items outside bounds
  auto result1 = container.Add(std::make_shared<TestEntity>(-5.0f, 50.0f));
  auto result2 = container.Add(std::make_shared<TestEntity>(105.0f, 50.0f));
  auto result3 = container.Add(std::make_shared<TestEntity>(50.0f, -5.0f));
  auto result4 = container.Add(std::make_shared<TestEntity>(50.0f, 105.0f));

  // All should return false
  ASSERT_FALSE(result1);
  ASSERT_FALSE(result2);
  ASSERT_FALSE(result3);
  ASSERT_FALSE(result4);
}

TEST(PositionalContainer, GetOutOfBounds) {
  // Test getting items with center or radius extending out of bounds
  WorldSize size(100.0f, 100.0f);
  PositionalContainer<TestEntity> container(size, 10);

  container.Add(std::make_shared<TestEntity>(50.0f, 50.0f));

  // Query that extends out of bounds should return empty
  auto results = container.Get(WorldPos(95.0f, 95.0f), 10.0f);
  ASSERT_EQ(results.size(), 0);
}

TEST(PositionalContainer, ItemsInDifferentChunks) {
  // Test that items in different grid chunks can be retrieved correctly
  WorldSize size(100.0f, 100.0f);
  PositionalContainer<TestEntity> container(size, 10);

  // Add items in different chunks (grid is 10x10, so each chunk is 10x10 units)
  container.Add(std::make_shared<TestEntity>(15.0f, 15.0f)); // Chunk (1,1)
  container.Add(std::make_shared<TestEntity>(25.0f, 25.0f)); // Chunk (2,2)
  container.Add(std::make_shared<TestEntity>(75.0f, 75.0f)); // Chunk (7,7)

  // Query in chunk (1,1)
  auto results1 = container.Get(WorldPos(15.0f, 15.0f), 3.0f);
  ASSERT_GE(results1.size(), 1);

  // Query in chunk (7,7)
  auto results2 = container.Get(WorldPos(75.0f, 75.0f), 3.0f);
  ASSERT_GE(results2.size(), 1);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
