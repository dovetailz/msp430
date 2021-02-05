#ifndef read_elf_test_h
#define read_elf_test_h

#include <iostream>
#include <string>

#include "gtest/gtest.h"
#include "read_elf.h"

class ReadElfTest : public ::testing::Test {
 public:
  ReadElfTest() : reader(DOCUMENT_PATH){};
  ~ReadElfTest(){};

  void SetUp(){};
  void TearDown(){};

  ElfReader reader;

 protected:
  uint8_t magic[4]{0x7f, 0x45, 0x4c, 0x46};
  std::vector<std::string> section_name{"",
                                        ".MSP430.attributes",
                                        ".bss",
                                        ".comment",
                                        ".data",
                                        ".debug_abbrev",
                                        ".debug_aranges",
                                        ".debug_frame",
                                        ".debug_info",
                                        ".debug_line",
                                        ".debug_loc",
                                        ".debug_ranges",
                                        ".debug_str",
                                        ".heap",
                                        ".noinit",
                                        ".rodata",
                                        ".rodata2",
                                        ".shstrtab",
                                        ".strtab",
                                        ".symtab",
                                        ".text",
                                        "__reset_vector"};
};

#endif