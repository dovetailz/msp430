#ifndef processor_test_h
#define processor_test_h

#include <iostream>

#include "gtest/gtest.h"
#include "memory.h"
#include "processor.h"

class ProcessorTest : public ::testing::Test {
 public:
  ProcessorTest(){};
  ~ProcessorTest(){};

  void SetUp();
  void TearDown(){};

  Memory mem;
  Processor proc;
};

#endif