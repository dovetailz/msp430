#include "gtest/gtest.h"

TEST(test, test) { ASSERT_FALSE(true); }

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}