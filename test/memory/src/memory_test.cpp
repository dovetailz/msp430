#include "memory_test.h"

TEST_F(MemoryTest, GetUint8) {

  std::cout<< mem.GetUint8(0) << std::endl;
  EXPECT_EQ(0,1) << "Val: " << mem.GetUint8(0) << std::endl; 
}