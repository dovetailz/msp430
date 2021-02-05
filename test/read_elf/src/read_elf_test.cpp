#include "read_elf_test.h"

TEST_F(ReadElfTest, NonExistentFile) {
  EXPECT_THROW(ElfReader test("/bad"), ElfReaderException)
      << "Elf file did not throw exception";

  try {
    ElfReader test("/bad");
  } catch (ElfReaderException& e) {
    EXPECT_STREQ(e.what(), "File does not exist") << "Invalid throw string";
  }
}

TEST_F(ReadElfTest, BadFile) {
  EXPECT_THROW(ElfReader test(TEXT_DOCUMENT_PATH), ElfReaderException)
      << "Elf file did not throw exception";

  try {
    ElfReader test(TEXT_DOCUMENT_PATH);
  } catch (ElfReaderException& e) {
    EXPECT_STREQ(e.what(), "Could not read file") << "Invalid throw string";
  }
}

TEST_F(ReadElfTest, Magic_Valid) {
  for (int x = 0; x < 4; x++) {
    ASSERT_EQ(reader.header.e_ident[x], magic[x]) << "MAGIC bytes incorrect";
  }
}

TEST_F(ReadElfTest, Magic_Invalid) {
  EXPECT_THROW(ElfReader test(PDF_DOCUMENT_PATH), ElfReaderException)
      << "Elf file did not throw exception";

  try {
    ElfReader test(PDF_DOCUMENT_PATH);
  } catch (ElfReaderException& e) {
    EXPECT_STREQ(e.what(), "Invalid magic bytes") << "Invalid throw string";
  }
}