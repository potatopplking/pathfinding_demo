#include <cassert>
#include <cmath>
#include <concepts>
#include <gtest/gtest.h>
#include <sstream>
#include <unordered_set>

#include "log.hpp"
#include "math.hpp"


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


TEST(vec, equalEpsilon) {
  // Test equalEpsilon
  // TODO just an ad-hoc test,
  // can possibly fail for other machines.
  // This needs some work
  vec3 v1{1.0f, 2.0f, 3.0f};
  vec3 v2{0.999999f, 1.9999999f, 2.9999999f};
  ASSERT_EQ(v1, v2);
}

TEST(vec, equalInt) {
  ivec2 v1{1,2};
  ivec2 v2{1,2};
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
  vec2  v2(1.2f, 3.4f);
  vec3  v3(1.2f, 3.4f, 5.6f);
  vec4  v4(1.2f, 3.4f, 5.6f, 7.8f);
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

TEST(vec, Add)
{
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

TEST(vec, Sub)
{
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

TEST(vec, ScalarMultiplication)
{
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

TEST(vec, ScalarDivision)
{
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

TEST(vec, AdditionAssignment)
{
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

TEST(vec, SubtractionAssignment)
{
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

TEST(vec, LengthSquared)
{
  // Test LengthSquared with float vectors
  vec3 v1{3.0f, 4.0f, 0.0f};
  ASSERT_FLOAT_EQ(v1.LengthSquared(), 25.0f); // 3² + 4² + 0² = 25
  
  vec2 v2{1.0f, 1.0f};
  ASSERT_FLOAT_EQ(v2.LengthSquared(), 2.0f); // 1² + 1² = 2
  
  // Test with zero vector
  vec3 zero{0.0f, 0.0f, 0.0f};
  ASSERT_FLOAT_EQ(zero.LengthSquared(), 0.0f);
}

TEST(vec, Length)
{
  // Test Length with float vectors
  vec3 v1{3.0f, 4.0f, 0.0f};
  ASSERT_FLOAT_EQ(v1.Length(), 5.0f); // sqrt(3² + 4² + 0²) = 5
  
  vec2 v2{1.0f, 1.0f};
  ASSERT_NEAR(v2.Length(), 1.414213f, 1e-5f); // sqrt(2) ≈ 1.414213
  
  // Test with zero vector
  vec3 zero{0.0f, 0.0f, 0.0f};
  ASSERT_FLOAT_EQ(zero.Length(), 0.0f);
}

TEST(vec, Normalize)
{
  // Test Normalize with float vectors
  vec3 v1{3.0f, 4.0f, 0.0f};
  v1.Normalize();
  
  ASSERT_FLOAT_EQ(v1[0], 0.6f);  // 3/5
  ASSERT_FLOAT_EQ(v1[1], 0.8f);  // 4/5
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

TEST(vec, GetNormalized)
{
  // Test GetNormalized with float vectors
  const vec3 v1{3.0f, 4.0f, 0.0f};
  vec3 normalized = v1.GetNormalized();
  
  // Original vector should be unchanged
  ASSERT_FLOAT_EQ(v1[0], 3.0f);
  ASSERT_FLOAT_EQ(v1[1], 4.0f);
  ASSERT_FLOAT_EQ(v1[2], 0.0f);
  
  // Normalized copy should be unit length
  ASSERT_FLOAT_EQ(normalized[0], 0.6f);  // 3/5
  ASSERT_FLOAT_EQ(normalized[1], 0.8f);  // 4/5
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

TEST(vec, GetOrthogonal)
{
  const vec2 v1{5.0f, 1.0f};
  auto v2 = v1.GetOrthogonal();
  ASSERT_FLOAT_EQ(v2[0], -1.0f);
  ASSERT_FLOAT_EQ(v2[1], 5.0f);
}

TEST(vec, DistanceTo)
{
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

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
