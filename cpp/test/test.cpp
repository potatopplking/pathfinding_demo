#include <cassert>
#include <cmath>
#include <concepts>
#include <gtest/gtest.h>
#include <sstream>
#include <unordered_set>

#include "array.hpp"
#include "log.hpp"
#include "math.hpp"

// Vec2D Tests
TEST(Vec2D, DefaultConstruction) {
  Vec2D<int> v;
  // Default values are uninitialized, but we can test basic functionality
  v.x = 0;
  v.y = 0;
  ASSERT_EQ(v.x, 0);
  ASSERT_EQ(v.y, 0);
}

TEST(Vec2D, InitializerListConstruction) {
  Vec2D<int> v{3, 4};
  ASSERT_EQ(v.x, 3);
  ASSERT_EQ(v.y, 4);

  Vec2D<float> vf{1.5f, 2.5f};
  ASSERT_FLOAT_EQ(vf.x, 1.5f);
  ASSERT_FLOAT_EQ(vf.y, 2.5f);

  // Test type conversion
  Vec2D<float> vd{1, 2}; // int to float
  ASSERT_FLOAT_EQ(vd.x, 1.0f);
  ASSERT_FLOAT_EQ(vd.y, 2.0f);
}

TEST(Vec2D, Addition) {
  Vec2D<int> a{1, 2};
  Vec2D<int> b{3, 4};

  Vec2D<int> c = a + b;
  ASSERT_EQ(c.x, 4);
  ASSERT_EQ(c.y, 6);

  // Test that original vectors are unchanged
  ASSERT_EQ(a.x, 1);
  ASSERT_EQ(a.y, 2);
  ASSERT_EQ(b.x, 3);
  ASSERT_EQ(b.y, 4);
}

TEST(Vec2D, AdditionAssignment) {
  Vec2D<int> a{1, 2};
  Vec2D<int> b{3, 4};

  a += b;
  ASSERT_EQ(a.x, 4);
  ASSERT_EQ(a.y, 6);

  // Test that b is unchanged
  ASSERT_EQ(b.x, 3);
  ASSERT_EQ(b.y, 4);
}

TEST(Vec2D, ScalarMultiplication) {
  Vec2D<int> v{2, 3};

  Vec2D<int> result = v * 2.0f;
  ASSERT_EQ(result.x, 4);
  ASSERT_EQ(result.y, 6);

  // Test with float vector
  Vec2D<float> vf{1.5f, 2.5f};
  Vec2D<float> resultf = vf * 2.0f;
  ASSERT_FLOAT_EQ(resultf.x, 3.0f);
  ASSERT_FLOAT_EQ(resultf.y, 5.0f);
}

TEST(Vec2D, Normalization) {
  Vec2D<float> v{3.0f, 4.0f}; // Length = 5

  v.normalize();
  ASSERT_FLOAT_EQ(v.x, 0.6f);
  ASSERT_FLOAT_EQ(v.y, 0.8f);

  // Check that length is approximately 1
  float length = sqrt(v.x * v.x + v.y * v.y);
  ASSERT_NEAR(length, 1.0f, 1e-6f);
}

TEST(Vec2D, NormalizedCopy) {
  Vec2D<float> v{3.0f, 4.0f};
  Vec2D<float> normalized = v.normalized();

  // Original should be unchanged
  ASSERT_FLOAT_EQ(v.x, 3.0f);
  ASSERT_FLOAT_EQ(v.y, 4.0f);

  // Normalized copy should be unit length
  ASSERT_FLOAT_EQ(normalized.x, 0.6f);
  ASSERT_FLOAT_EQ(normalized.y, 0.8f);

  float length =
      sqrt(normalized.x * normalized.x + normalized.y * normalized.y);
  ASSERT_NEAR(length, 1.0f, 1e-6f);
}

TEST(Vec2D, ZeroVectorNormalization) {
  Vec2D<float> v{0.0f, 0.0f};

  v.normalize();
  ASSERT_FLOAT_EQ(v.x, 0.0f);
  ASSERT_FLOAT_EQ(v.y, 0.0f);

  // Test normalized() as well
  Vec2D<float> v2{0.0f, 0.0f};
  Vec2D<float> normalized = v2.normalized();
  ASSERT_FLOAT_EQ(normalized.x, 0.0f);
  ASSERT_FLOAT_EQ(normalized.y, 0.0f);
}

TEST(Vec2D, VerySmallVectorNormalization) {
  Vec2D<float> v{1e-7f, 1e-7f}; // Very small vector

  v.normalize();
  // Should be treated as zero vector
  ASSERT_FLOAT_EQ(v.x, 0.0f);
  ASSERT_FLOAT_EQ(v.y, 0.0f);
}

TEST(Vec2D, OutputOperator) {
  Vec2D<int> v{42, 24};

  std::ostringstream oss;
  oss << v;
  ASSERT_EQ(oss.str(), "( 42, 24)");
}

TEST(Vec2D, ChainedOperations) {
  Vec2D<float> a{1.0f, 2.0f};
  Vec2D<float> b{3.0f, 4.0f};

  // Test chaining: (a + b) * 2.0f
  Vec2D<float> result = (a + b) * 2.0f;
  ASSERT_FLOAT_EQ(result.x, 8.0f);
  ASSERT_FLOAT_EQ(result.y, 12.0f);

  // Test chaining with assignment
  a += b;
  a = a * 0.5f;
  ASSERT_FLOAT_EQ(a.x, 2.0f);
  ASSERT_FLOAT_EQ(a.y, 3.0f);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
