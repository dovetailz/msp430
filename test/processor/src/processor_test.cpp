#include "processor_test.h"

void ProcessorTest::SetUp() {
  mem.LoadFile(DOCUMENT_PATH);
  proc.SetMemory(&mem);
}

TEST_F(ProcessorTest, Init_PC) {
  EXPECT_EQ(*proc.PC, 0xfc00) << "PC not initialized correctly";
  EXPECT_EQ(*proc.PC, mem.GetUint16(proc.RESET_VECTOR))
      << "PC not initialized correctly";
}

TEST_F(ProcessorTest, Init_SR) {
  EXPECT_EQ(proc.SR->val, 0x0000) << "Status register not initialized properly";
}

TEST_F(ProcessorTest, Step) {
  proc.Step();
  proc.Step();
  proc.Step();
  proc.Step();
  proc.Step();
  proc.Step();
  proc.Step();
}